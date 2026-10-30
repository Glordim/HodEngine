#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Button.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>
#include <HodEngine/UI2/LayoutParamsFactory.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

#include <map>
#include <memory>

namespace hod
{
	namespace
	{
		// The prefabs' documents, as UIPrefabResource holds them
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

		Node* NewNode(const char* name)
		{
			Node* node = DefaultAllocator::GetInstance().New<Node>();
			node->SetName(name);
			return node;
		}

		// The entry of a new instance of prefab, as the editor makes it
		void WriteInstanceEntry(DocumentNode& entry, const UID& prefab)
		{
			entry.AddChild("PrefabInstance").AddChild("Prefab").SetString(prefab.ToString());
		}

		// An override of target's Name
		PropertyOverride MakeNameOverride(uint64_t target, const char* name)
		{
			Node scratch;
			scratch.SetName(name);
			PropertyValue value;
			value.Read(scratch, "Name");

			PropertyOverride propertyOverride;
			propertyOverride._target = target;
			propertyOverride._property = "Name";
			propertyOverride.SetValue(value);
			return propertyOverride;
		}
	}

	class PrefabInstanceTest : public ::testing::Test
	{
	protected:
		static void SetUpTestSuite()
		{
			NodeFactory::CreateInstance()->Register<Node>();
			LayoutParamsFactory::CreateInstance()->Register<AnchoredLayoutParams>();
			LayoutFactory::CreateInstance();
			InteractionFactory::CreateInstance()->Register<Button>();
		}

		static void TearDownTestSuite()
		{
			InteractionFactory::DestroyInstance();
			LayoutFactory::DestroyInstance();
			LayoutParamsFactory::DestroyInstance();
			NodeFactory::DestroyInstance();
		}

		void SetUp() override
		{
			PrefabInstance::SetSourceProvider(&LoadPrefab);
		}

		void TearDown() override
		{
			PrefabInstance::SetSourceProvider(nullptr);
			prefabDocuments.clear();
		}

		// Saves root as a prefab: in a container as a canvas holds it, with ids from 1 (container 1, root 2, then depth-first)
		static UID AddPrefab(Node* root)
		{
			Node container;
			container.AddChild(root);
			uint64_t nextLocalId = 1;
			container.AssignLocalIds(nextLocalId);

			std::unique_ptr<Document> document = std::make_unique<Document>();
			EXPECT_TRUE(container.SerializeInDocument(document->GetRootNode()));

			UID prefab = UID::GenerateUID();
			prefabDocuments[prefab] = std::move(document);
			return prefab;
		}
	};

	TEST_F(PrefabInstanceTest, InstanceFollowsItsPrefabAndKeepsItsOverrides)
	{
		Node* root = NewNode("Root");
		root->AddChild(NewNode("Label"));
		UID prefab = AddPrefab(root);

		Node     outer;
		Document entry;
		WriteInstanceEntry(entry.GetRootNode(), prefab);
		Node* instance = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);
		ASSERT_NE(instance->GetPrefabInstance(), nullptr);
		EXPECT_FALSE(instance->GetPrefabInstance()->IsMissing());
		EXPECT_TRUE(instance->GetName() == "Root");
		ASSERT_EQ(instance->GetChildren().Size(), 1u);
		EXPECT_TRUE(instance->GetChildren()[0]->GetName() == "Label");

		// Nothing differs yet
		Document saved;
		ASSERT_TRUE(instance->SerializeAsChildInDocument(saved.GetRootNode()));
		EXPECT_EQ(saved.GetRootNode().GetChild("PrefabInstance")->GetChild("Overrides")->GetFirstChild(), nullptr);

		instance->GetChildren()[0]->SetName("Overridden");
		Document overridden;
		ASSERT_TRUE(instance->SerializeAsChildInDocument(overridden.GetRootNode()));
		const DocumentNode* overrides = overridden.GetRootNode().GetChild("PrefabInstance")->GetChild("Overrides");
		ASSERT_NE(overrides->GetFirstChild(), nullptr);
		EXPECT_EQ(overrides->GetFirstChild()->GetNextSibling(), nullptr);
		// Only the override and the placement are saved, not the content
		EXPECT_EQ(overridden.GetRootNode().GetChild("Children"), nullptr);

		Node  reloadedOuter;
		Node* reloaded = reloadedOuter.AddChildFromDocument(overridden.GetRootNode());
		ASSERT_NE(reloaded, nullptr);
		ASSERT_EQ(reloaded->GetChildren().Size(), 1u);
		EXPECT_TRUE(reloaded->GetChildren()[0]->GetName() == "Overridden");
		EXPECT_TRUE(reloaded->GetName() == "Root");
	}

	TEST_F(PrefabInstanceTest, LocalIdsAreScopedPerInstance)
	{
		Node* root = NewNode("Root"); // id 2 in its prefab
		root->AddChild(NewNode("Label")); // id 3
		UID prefab = AddPrefab(root);

		Node  outer;
		Node* own = NewNode("Own");
		outer.AddChild(own);
		Document entry;
		WriteInstanceEntry(entry.GetRootNode(), prefab);
		Node* instance = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);
		Node* label = instance->GetChildren()[0];

		uint64_t nextLocalId = 1;
		outer.AssignLocalIds(nextLocalId); // outer 1, own 2, instance 3: the same ids as the prefab's nodes
		EXPECT_EQ(own->GetLocalId(), 2u);
		EXPECT_EQ(instance->GetLocalId(), 3u);
		EXPECT_EQ(label->GetLocalId(), 3u); // Its prefab's, untouched

		EXPECT_EQ(outer.FindByLocalId(2), own);
		EXPECT_EQ(outer.FindByLocalId(3), instance);
		EXPECT_EQ(instance->FindByLocalId(2), instance);
		EXPECT_EQ(instance->FindByLocalId(3), label);

		EXPECT_EQ(own->GetScopeRoot(), &outer);
		EXPECT_EQ(instance->GetScopeRoot(), instance);
		EXPECT_EQ(label->GetScopeRoot(), instance);
	}

	TEST_F(PrefabInstanceTest, UnpackGivesNewIdsTheReferencesFollow)
	{
		Node* root = NewNode("Root"); // id 2
		Node* label = NewNode("Label"); // id 3
		root->AddChild(label);
		Button& button = root->SetInteraction<Button>();
		button.GetOverrides(Button::State::Hovered)->PushBack(MakeNameOverride(3, "Hovered"));
		UID prefab = AddPrefab(root);

		Node     outer;
		Document entry;
		WriteInstanceEntry(entry.GetRootNode(), prefab);
		Node* instance = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);

		uint64_t nextLocalId = 10;
		outer.AssignLocalIds(nextLocalId); // outer 10, instance 11
		instance->UnpackPrefabInstance(nextLocalId);

		EXPECT_EQ(instance->GetPrefabInstance(), nullptr);
		EXPECT_EQ(instance->GetLocalId(), 11u);
		Node* unpackedLabel = instance->GetChildren()[0];
		EXPECT_EQ(unpackedLabel->GetLocalId(), 12u);
		EXPECT_EQ(outer.FindByLocalId(12), unpackedLabel);

		Button* unpackedButton = instance->GetInteraction<Button>();
		ASSERT_NE(unpackedButton, nullptr);
		ASSERT_EQ(unpackedButton->GetOverrides(Button::State::Hovered)->Size(), 1u);
		EXPECT_EQ((*unpackedButton->GetOverrides(Button::State::Hovered))[0]._target, 12u);
	}

	TEST_F(PrefabInstanceTest, CopyRenewsOnlyTheContainingPrefabsIds)
	{
		Node* root = NewNode("Root");
		root->AddChild(NewNode("Label")); // id 3
		UID prefab = AddPrefab(root);

		Node     outer;
		Document entry;
		WriteInstanceEntry(entry.GetRootNode(), prefab);
		Node* instance = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);

		uint64_t nextLocalId = 20;
		instance->RenewLocalIds(nextLocalId, true);
		EXPECT_EQ(instance->GetLocalId(), 20u);
		EXPECT_EQ(nextLocalId, 21u);
		EXPECT_EQ(instance->GetChildren()[0]->GetLocalId(), 3u);
	}

	TEST_F(PrefabInstanceTest, PrefabContainingItselfIsLeftEmpty)
	{
		// A prefab whose root holds an instance of itself
		UID                       prefab = UID::GenerateUID();
		std::unique_ptr<Document> document = std::make_unique<Document>();
		DocumentNode&             rootEntry = document->GetRootNode().AddChild("Children").AddChild("");
		WriteInstanceEntry(rootEntry.AddChild("Children").AddChild(""), prefab);
		prefabDocuments[prefab] = std::move(document);

		EXPECT_TRUE(PrefabInstance::Contains(prefab, prefab));

		Node     outer;
		Document entry;
		WriteInstanceEntry(entry.GetRootNode(), prefab);
		Node* instance = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);
		EXPECT_FALSE(instance->GetPrefabInstance()->IsMissing());
		ASSERT_EQ(instance->GetChildren().Size(), 1u);
		ASSERT_NE(instance->GetChildren()[0]->GetPrefabInstance(), nullptr);
		EXPECT_TRUE(instance->GetChildren()[0]->GetPrefabInstance()->IsMissing());
	}

	TEST_F(PrefabInstanceTest, ContainsFollowsNestedPrefabs)
	{
		UID inner = AddPrefab(NewNode("Inner"));

		std::unique_ptr<Document> document = std::make_unique<Document>();
		DocumentNode&             rootEntry = document->GetRootNode().AddChild("Children").AddChild("");
		WriteInstanceEntry(rootEntry.AddChild("Children").AddChild(""), inner);
		UID outerPrefab = UID::GenerateUID();
		prefabDocuments[outerPrefab] = std::move(document);

		EXPECT_TRUE(PrefabInstance::Contains(outerPrefab, inner));
		EXPECT_FALSE(PrefabInstance::Contains(inner, outerPrefab));
	}

	TEST_F(PrefabInstanceTest, MissingPrefabKeepsItsData)
	{
		Document      entry;
		DocumentNode& instanceNode = entry.GetRootNode().AddChild("PrefabInstance");
		instanceNode.AddChild("Prefab").SetString(UID::GenerateUID().ToString());
		ASSERT_TRUE(Serializer::Serialize(MakeNameOverride(5, "Kept"), instanceNode.AddChild("Overrides").AddChild("")));

		Node  outer;
		Node* placeholder = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(placeholder, nullptr);
		ASSERT_NE(placeholder->GetPrefabInstance(), nullptr);
		EXPECT_TRUE(placeholder->GetPrefabInstance()->IsMissing());
		EXPECT_TRUE(placeholder->GetChildren().Empty());

		Document saved;
		ASSERT_TRUE(placeholder->SerializeAsChildInDocument(saved.GetRootNode()));
		const DocumentNode* overrides = saved.GetRootNode().GetChild("PrefabInstance")->GetChild("Overrides");
		ASSERT_NE(overrides->GetFirstChild(), nullptr);
		EXPECT_EQ(overrides->GetFirstChild()->GetChild("Target")->GetUInt64(), 5u);
	}
}
