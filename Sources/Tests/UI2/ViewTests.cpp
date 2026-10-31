#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>
#include <HodEngine/UI2/LayoutParamsFactory.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
#include <HodEngine/UI2/NodeRef.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/UI2/View.hpp>
#include <HodEngine/UI2/ViewFactory.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace hod
{
	// A node type of its own, for the references expecting one
	class LabelNode : public Node
	{
		REFLECTED_CLASS(LabelNode, Node)
	};

	DESCRIBE_REFLECTED_CLASS(LabelNode, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	// What the views went through, in order: "Bind <name of its node>", "Unbind <...>"
	static std::vector<std::string> viewEvents;

	class TestView : public View
	{
		REFLECTED_CLASS(TestView, View)

	public:
		NodeRef<LabelNode>	_label;
		NodeRef<Node>		_panel;
		ViewRef<TestView>	_nested;

		// Seen from OnBind
		bool				_nestedBoundFirst = false;

	protected:
		void OnBind() override
		{
			viewEvents.push_back(std::string("Bind ") + GetNode()->GetName().CStr());
			_nestedBoundFirst = _nested.Get() != nullptr && _nested->IsBound();
		}

		void OnUnbind() override
		{
			viewEvents.push_back(std::string("Unbind ") + GetNode()->GetName().CStr());
		}
	};

	DESCRIBE_REFLECTED_CLASS(TestView, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &TestView::_label, "Label");
		AddPropertyT(reflectionDescriptor, &TestView::_panel, "Panel");
		AddPropertyT(reflectionDescriptor, &TestView::_nested, "Nested");
	}

	namespace
	{
		std::map<UID, std::unique_ptr<Document>> prefabDocuments;

		bool LoadPrefab(const UID& prefab, Document& outDocument)
		{
			auto it = prefabDocuments.find(prefab);
			if (it == prefabDocuments.end())
			{
				return false;
			}
			outDocument.GetRootNode().Copy(it->second->GetRootNode());
			return true;
		}

		template<typename _Node_ = Node>
		_Node_* NewNode(const char* name, Node* parent)
		{
			_Node_* node = DefaultAllocator::GetInstance().New<_Node_>();
			node->SetName(name);
			if (parent != nullptr)
			{
				parent->AddChild(node);
			}
			return node;
		}

		// Root (TestView) > Panel > Label, in a container as a canvas holds it, ids given. The view references Panel and Label.
		Node* BuildPrefab(Node& container, const char* rootName = "Root")
		{
			Node*      root = NewNode(rootName, &container);
			Node*      panel = NewNode("Panel", root);
			LabelNode* label = NewNode<LabelNode>("Label", panel);
			uint64_t   nextLocalId = 1;
			container.AssignLocalIds(nextLocalId);

			TestView& view = root->SetView<TestView>();
			view._panel.SetLocalId(panel->GetLocalId());
			view._label.SetLocalId(label->GetLocalId());
			return root;
		}

		// The container's content as its document (a prefab's), and back in a new container
		std::unique_ptr<Document> Save(Node& container)
		{
			std::unique_ptr<Document> document = std::make_unique<Document>();
			EXPECT_TRUE(container.SerializeInDocument(document->GetRootNode()));
			return document;
		}
	}

	class ViewTest : public ::testing::Test
	{
	protected:
		static void SetUpTestSuite()
		{
			NodeFactory* nodeFactory = NodeFactory::CreateInstance();
			nodeFactory->Register<Node>();
			nodeFactory->Register<LabelNode>();
			LayoutParamsFactory::CreateInstance()->Register<AnchoredLayoutParams>();
			LayoutFactory::CreateInstance();
			InteractionFactory::CreateInstance();
			ViewFactory::CreateInstance();
		}

		static void TearDownTestSuite()
		{
			ViewFactory::DestroyInstance();
			InteractionFactory::DestroyInstance();
			LayoutFactory::DestroyInstance();
			LayoutParamsFactory::DestroyInstance();
			NodeFactory::DestroyInstance();
		}

		void SetUp() override
		{
			ViewFactory::GetInstance()->Register<TestView>();
			PrefabInstance::SetSourceProvider(&LoadPrefab);
			viewEvents.clear();
		}

		void TearDown() override
		{
			PrefabInstance::SetSourceProvider(nullptr);
			prefabDocuments.clear();
			ViewFactory::GetInstance()->Unregister<TestView>();
		}
	};

	TEST_F(ViewTest, ReferencesFollowTheNodesRenamedOrMoved)
	{
		Node  container;
		Node* root = BuildPrefab(container);
		Node* panel = root->GetChildren()[0];
		Node* label = panel->GetChildren()[0];

		label->SetName("Renamed");
		panel->ReparentChild(label, root);

		TestView* view = root->GetView<TestView>();
		ASSERT_NE(view, nullptr);
		EXPECT_EQ(view->_label.Get(), nullptr); // Not bound yet

		container.BindViews();
		EXPECT_TRUE(view->IsBound());
		EXPECT_EQ(view->_label.Get(), label);
		EXPECT_EQ(view->_panel.Get(), panel);
		EXPECT_EQ(view->_nested.Get(), nullptr); // No reference
		EXPECT_EQ(viewEvents, std::vector<std::string>({"Bind Root"}));

		container.UnbindViews();
		EXPECT_FALSE(view->IsBound());
		EXPECT_EQ(view->_label.Get(), nullptr);
		EXPECT_EQ(viewEvents, std::vector<std::string>({"Bind Root", "Unbind Root"}));
	}

	TEST_F(ViewTest, ReferenceToANodeOfAnotherTypeStaysNull)
	{
		Node  container;
		Node* root = BuildPrefab(container);
		Node* panel = root->GetChildren()[0];

		TestView* view = root->GetView<TestView>();
		view->_label.SetLocalId(panel->GetLocalId()); // A plain Node, not a LabelNode
		EXPECT_FALSE(view->_label.Accepts(*panel));

		container.BindViews();
		EXPECT_EQ(view->_label.Get(), nullptr);
		EXPECT_EQ(view->_panel.Get(), panel);
	}

	TEST_F(ViewTest, ViewIsSavedAndLoadedWithItsReferences)
	{
		Node container;
		BuildPrefab(container);
		std::unique_ptr<Document> document = Save(container);

		Node loaded;
		ASSERT_TRUE(loaded.DeserializeFromDocument(document->GetRootNode()));
		ASSERT_EQ(loaded.GetChildren().Size(), 1u);
		Node*     root = loaded.GetChildren()[0];
		TestView* view = root->GetView<TestView>();
		ASSERT_NE(view, nullptr);

		loaded.BindViews();
		EXPECT_EQ(view->_panel.Get(), root->GetChildren()[0]);
		EXPECT_EQ(view->_label.Get(), root->GetChildren()[0]->GetChildren()[0]);
	}

	TEST_F(ViewTest, ViewOfAnUnknownTypeIsKeptAsIs)
	{
		Node container;
		BuildPrefab(container);
		std::unique_ptr<Document> document = Save(container);

		// Its module not loaded: loaded and saved again without it
		ViewFactory::GetInstance()->Unregister<TestView>();
		Node withoutModule;
		ASSERT_TRUE(withoutModule.DeserializeFromDocument(document->GetRootNode()));
		Node* root = withoutModule.GetChildren()[0];
		EXPECT_EQ(root->GetView(), nullptr);
		EXPECT_TRUE(root->HasUnknownView());
		std::unique_ptr<Document> savedWithout = Save(withoutModule);

		ViewFactory::GetInstance()->Register<TestView>();
		Node withModule;
		ASSERT_TRUE(withModule.DeserializeFromDocument(savedWithout->GetRootNode()));
		root = withModule.GetChildren()[0];
		TestView* view = root->GetView<TestView>();
		ASSERT_NE(view, nullptr);
		EXPECT_FALSE(root->HasUnknownView());
		withModule.BindViews();
		EXPECT_EQ(view->_panel.Get(), root->GetChildren()[0]);
	}

	TEST_F(ViewTest, CopyReferencesItsOwnNodes)
	{
		Node  container;
		Node* root = BuildPrefab(container);
		Node* panel = root->GetChildren()[0];

		// The panel copied with a view referencing its label (inside the copy) and the root (outside)
		LabelNode* label = static_cast<LabelNode*>(panel->GetChildren()[0]);
		TestView&  panelView = panel->SetView<TestView>();
		panelView._label.SetLocalId(label->GetLocalId());
		panelView._panel.SetLocalId(root->GetLocalId());

		Document entry;
		ASSERT_TRUE(panel->SerializeAsChildInDocument(entry.GetRootNode()));
		Node* copy = root->AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(copy, nullptr);
		uint64_t nextLocalId = 100;
		copy->RenewLocalIds(nextLocalId, false);

		TestView* copyView = copy->GetView<TestView>();
		ASSERT_NE(copyView, nullptr);
		EXPECT_EQ(copyView->_label.GetLocalId(), copy->GetChildren()[0]->GetLocalId());
		EXPECT_NE(copyView->_label.GetLocalId(), label->GetLocalId());
		EXPECT_EQ(copyView->_panel.GetLocalId(), 0u); // Outside of the copy: dropped
	}

	TEST_F(ViewTest, NestedViewsAreBoundFirstAndUnboundAfter)
	{
		// The nested prefab: Root (TestView) > Panel > Label
		Node nestedContainer;
		BuildPrefab(nestedContainer, "Nested");
		UID prefab = UID::GenerateUID();
		prefabDocuments[prefab] = Save(nestedContainer);

		// Outer (TestView) > instance of it, referenced by the outer view
		Node* container = DefaultAllocator::GetInstance().New<Node>();
		Node* outer = NewNode("Outer", container);
		Document entry;
		entry.GetRootNode().AddChild("PrefabInstance").AddChild("Prefab").SetString(prefab.ToString());
		Node* instance = outer->AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);
		uint64_t nextLocalId = 1;
		container->AssignLocalIds(nextLocalId);
		TestView& outerView = outer->SetView<TestView>();
		outerView._nested.SetLocalId(instance->GetLocalId());

		container->BindViews();
		ASSERT_NE(outerView._nested.Get(), nullptr);
		EXPECT_TRUE(outerView._nestedBoundFirst);
		// The nested view's references are resolved in its own prefab
		EXPECT_EQ(outerView._nested->_panel.Get(), instance->GetChildren()[0]);
		EXPECT_EQ(viewEvents, std::vector<std::string>({"Bind Nested", "Bind Outer"}));

		// Deleting the nodes unbinds their views, the highest first
		viewEvents.clear();
		DefaultAllocator::GetInstance().Delete(container);
		EXPECT_EQ(viewEvents, std::vector<std::string>({"Unbind Outer", "Unbind Nested"}));
	}
}
