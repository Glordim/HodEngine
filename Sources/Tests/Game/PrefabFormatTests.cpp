#include <gtest/gtest.h>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Prefab.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneSerializer.hpp>
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod
{
	/// @brief A prefab document as the editor writes it (see PrefabEditorTab), the root entity first
	static void WritePrefab(Entity* root, Document& document)
	{
		uint64_t        nextLocalId = 1;
		SceneSerializer sceneSerializer;
		document.GetRootNode().AddChild("Name").SetString("Goblin");
		ASSERT_TRUE(sceneSerializer.SerializeEntity(root, true, document.GetRootNode().AddChild("Entities"), nextLocalId));
		document.GetRootNode().AddChild("NextLocalId").SetUInt64(nextLocalId);
	}

	// What the editor reads in a prefab's document to find its root and rename it (PrefabEditorTab::NameRootEntityAfterAsset):
	// the entity whose Parent's Id is 0, its name in "_name"
	TEST(PrefabFormat, RootIsTheEntityWithoutParent)
	{
		Entity* root = DefaultAllocator::GetInstance().New<Entity>("Goblin");
		Entity* child = DefaultAllocator::GetInstance().New<Entity>("Sword");
		child->SetParent(root);

		Document document;
		WritePrefab(root, document);

		const DocumentNode* entitiesNode = document.GetRootNode().GetChild("Entities");
		ASSERT_NE(entitiesNode, nullptr);
		const DocumentNode* rootNode = entitiesNode->GetFirstChild();
		ASSERT_NE(rootNode, nullptr);
		const DocumentNode* childNode = rootNode->GetNextSibling();
		ASSERT_NE(childNode, nullptr);

		ASSERT_NE(rootNode->GetChild("Parent"), nullptr);
		ASSERT_NE(rootNode->GetChild("Parent")->GetChild("Id"), nullptr);
		EXPECT_EQ(rootNode->GetChild("Parent")->GetChild("Id")->GetUInt64(), 0u);
		ASSERT_NE(rootNode->GetChild("_name"), nullptr);
		EXPECT_EQ(rootNode->GetChild("_name")->GetString(), "Goblin");

		ASSERT_NE(childNode->GetChild("Parent"), nullptr);
		EXPECT_EQ(childNode->GetChild("Parent")->GetChild("Id")->GetUInt64(), root->GetLocalId());

		// Renamed in the document, the prefab loads with that name
		const_cast<DocumentNode*>(rootNode)->GetOrAddChild("_name").SetString("Orc");
		Prefab prefab;
		ASSERT_TRUE(prefab.DeserializeFromDocument(document.GetRootNode()));
		ASSERT_NE(prefab.GetRootEntity(), nullptr);
		EXPECT_EQ(prefab.GetRootEntity()->GetName(), "Orc");
		ASSERT_EQ(prefab.GetRootEntity()->GetChildren().Size(), 1u);

		DefaultAllocator::GetInstance().Delete(child);
		DefaultAllocator::GetInstance().Delete(root);
	}

	TEST(Entity, SetSiblingIndexBothWays)
	{
		Entity* parent = DefaultAllocator::GetInstance().New<Entity>("Parent");
		Entity* children[3];
		for (uint32_t i = 0; i < 3; ++i)
		{
			children[i] = DefaultAllocator::GetInstance().New<Entity>("Child");
			children[i]->SetParent(parent);
		}

		// Towards the end: 1 2 0
		children[0]->SetSiblingIndex(2);
		ASSERT_EQ(parent->GetChildren().Size(), 3u);
		EXPECT_EQ(parent->GetChildren()[0].Lock(), children[1]);
		EXPECT_EQ(parent->GetChildren()[1].Lock(), children[2]);
		EXPECT_EQ(parent->GetChildren()[2].Lock(), children[0]);

		// Towards the start: 0 1 2
		children[0]->SetSiblingIndex(0);
		EXPECT_EQ(parent->GetChildren()[0].Lock(), children[0]);
		EXPECT_EQ(parent->GetChildren()[1].Lock(), children[1]);
		EXPECT_EQ(parent->GetChildren()[2].Lock(), children[2]);

		for (Entity* child : children)
		{
			DefaultAllocator::GetInstance().Delete(child);
		}
		DefaultAllocator::GetInstance().Delete(parent);
	}

	// What the editor's paste does (EntityEditorTab::PasteEntities): the copied subtrees added to a scene, their roots in the copied order
	TEST(SceneInstantiate, FromDocumentKeepsRootsOrderAndChildren)
	{
		Entity* first = DefaultAllocator::GetInstance().New<Entity>("First");
		Entity* child = DefaultAllocator::GetInstance().New<Entity>("Child");
		child->SetParent(first);
		Entity* second = DefaultAllocator::GetInstance().New<Entity>("Second");

		Document        document;
		uint64_t        nextLocalId = 1;
		SceneSerializer sceneSerializer;
		ASSERT_TRUE(sceneSerializer.SerializeEntity(first, true, document.GetRootNode(), nextLocalId));
		ASSERT_TRUE(sceneSerializer.SerializeEntity(second, true, document.GetRootNode(), nextLocalId));

		// Not initialized: no frame job nor systems needed to activate entities
		World* world = DefaultAllocator::GetInstance().New<World>();
		Scene* scene = DefaultAllocator::GetInstance().New<Scene>();
		world->AddScene(scene);

		Vector<Entity*> roots = scene->Instantiate(document.GetRootNode());
		ASSERT_EQ(roots.Size(), 2u);
		EXPECT_EQ(roots[0]->GetName(), "First");
		EXPECT_EQ(roots[1]->GetName(), "Second");
		ASSERT_EQ(roots[0]->GetChildren().Size(), 1u);
		EXPECT_EQ(roots[0]->GetChildren()[0].Lock()->GetName(), "Child");
		EXPECT_EQ(scene->GetEntities().size(), 3u);
		EXPECT_EQ(roots[0]->GetScene(), scene);

		world->RemoveScene(scene);
		DefaultAllocator::GetInstance().Delete(scene);
		DefaultAllocator::GetInstance().Delete(world);

		DefaultAllocator::GetInstance().Delete(child);
		DefaultAllocator::GetInstance().Delete(first);
		DefaultAllocator::GetInstance().Delete(second);
	}
}
