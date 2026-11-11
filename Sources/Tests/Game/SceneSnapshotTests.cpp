#include <gtest/gtest.h>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/String.hpp>

namespace hod
{
	class SceneSnapshotTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Not initialized: no frame job nor systems needed to activate entities
			_world = DefaultAllocator::GetInstance().New<World>();
			_scene = DefaultAllocator::GetInstance().New<Scene>();
			_world->AddScene(_scene);
		}

		void TearDown() override
		{
			_world->RemoveScene(_scene);
			DefaultAllocator::GetInstance().Delete(_scene);
			DefaultAllocator::GetInstance().Delete(_world);
		}

		String Write()
		{
			Document document;
			EXPECT_TRUE(_scene->SerializeInDocument(document.GetRootNode()));
			String             text;
			DocumentWriterJson documentWriter;
			EXPECT_TRUE(documentWriter.Write(document, text));
			return text;
		}

		World* _world = nullptr;
		Scene* _scene = nullptr;
	};

	// The editor's undo (EntityEditorTab::CaptureUndoState) compares the written states to tell an edit from nothing: the scene rebuilt
	// from what it wrote (an undo) must write it again the same way, whatever the order of its unordered entities
	TEST_F(SceneSnapshotTest, RebuiltSceneWritesTheSameState)
	{
		for (const char* name : {"A", "B", "C", "D", "E"})
		{
			Entity* root = _scene->CreateEntity(name);
			Entity* child = _scene->CreateEntity("Child");
			child->SetParent(root);
		}

		String written = Write();

		Document document;
		ASSERT_TRUE(_scene->SerializeInDocument(document.GetRootNode()));
		_scene->DestroyEntities();
		EXPECT_TRUE(_scene->GetEntities().empty());
		ASSERT_TRUE(_scene->DeserializeFromDocument(document.GetRootNode()));
		_scene->ProcessActivation();

		EXPECT_EQ(_scene->GetEntities().size(), 10u);
		EXPECT_EQ(Write(), written);

		// The roots keep their order
		Vector<Entity*> roots = _scene->GetRootEntities();
		ASSERT_EQ(roots.Size(), 5u);
		EXPECT_EQ(roots[0]->GetName(), "A");
		EXPECT_EQ(roots[4]->GetName(), "E");
	}

	// The roots have an order of their own (the hierarchy's), changed by SetSiblingIndex, kept by a snapshot
	TEST_F(SceneSnapshotTest, RootsReordered)
	{
		Entity* a = _scene->CreateEntity("A");
		Entity* b = _scene->CreateEntity("B");
		Entity* c = _scene->CreateEntity("C");

		c->SetSiblingIndex(0);
		EXPECT_EQ(c->GetSiblingIndex(), 0u);
		Vector<Entity*> roots = _scene->GetRootEntities();
		ASSERT_EQ(roots.Size(), 3u);
		EXPECT_EQ(roots[0], c);
		EXPECT_EQ(roots[1], a);
		EXPECT_EQ(roots[2], b);

		// Out of range: the last place
		c->SetSiblingIndex(UINT32_MAX);
		EXPECT_EQ(c->GetSiblingIndex(), 2u);
		c->SetSiblingIndex(0);

		Document document;
		ASSERT_TRUE(_scene->SerializeInDocument(document.GetRootNode()));
		_scene->DestroyEntities();
		ASSERT_TRUE(_scene->DeserializeFromDocument(document.GetRootNode()));
		roots = _scene->GetRootEntities();
		ASSERT_EQ(roots.Size(), 3u);
		EXPECT_EQ(roots[0]->GetName(), "C");
		EXPECT_EQ(roots[1]->GetName(), "A");
		EXPECT_EQ(roots[2]->GetName(), "B");
	}

	// An entity given a parent leaves the roots, one losing its parent comes back last, a destroyed one leaves them
	TEST_F(SceneSnapshotTest, RootsFollowParents)
	{
		Entity* a = _scene->CreateEntity("A");
		Entity* b = _scene->CreateEntity("B");
		Entity* c = _scene->CreateEntity("C");

		a->SetParent(b);
		Vector<Entity*> roots = _scene->GetRootEntities();
		ASSERT_EQ(roots.Size(), 2u);
		EXPECT_EQ(roots[0], b);
		EXPECT_EQ(roots[1], c);

		a->SetParent(WeakEntity(nullptr));
		roots = _scene->GetRootEntities();
		ASSERT_EQ(roots.Size(), 3u);
		EXPECT_EQ(roots[2], a);

		_scene->DestroyEntity(c);
		roots = _scene->GetRootEntities();
		ASSERT_EQ(roots.Size(), 2u);
		EXPECT_EQ(roots[0], b);
		EXPECT_EQ(roots[1], a);
		DefaultAllocator::GetInstance().Delete(c);
	}
}
