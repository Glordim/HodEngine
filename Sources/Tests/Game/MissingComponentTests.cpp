#include <gtest/gtest.h>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/ComponentFactory.hpp>
#include <HodEngine/Game/Components/MissingComponent.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/SceneSerializer.hpp>

#include <HodEngine/Core/Document/Document.hpp>

namespace hod
{
	class MissingComponentTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			ComponentFactory::CreateInstance(); // Empty: every component type is unknown
		}

		void TearDown() override
		{
			ComponentFactory::DestroyInstance();
		}

		/// @brief An entity with a component of an unknown type (a disabled module's)
		static void FillEntities(DocumentNode& entitiesNode)
		{
			DocumentNode& entityNode = entitiesNode.AddChild("");
			entityNode.AddChild("_localId").SetUInt64(1);
			entityNode.AddChild("_name").SetString("Body");
			DocumentNode& componentNode = entityNode.AddChild("Components").AddChild("");
			componentNode.AddChild("MetaType").SetUInt64(0x1234567890ull);
			componentNode.AddChild("_localId").SetUInt64(2);
			componentNode.AddChild("_enabled").SetBool(false);
			componentNode.AddChild("GravityScale").SetFloat32(2.5f);
			componentNode.AddChild("Mode").SetInt32(2);
		}
	};

	TEST_F(MissingComponentTest, UnknownComponentIsKeptAndWrittenBack)
	{
		Document source;
		FillEntities(source.GetRootNode());

		SceneSerializer loader;
		ASSERT_TRUE(loader.Deserialize(source.GetRootNode()));
		ASSERT_EQ(loader.GetEntities().Size(), 1u);
		Entity* entity = loader.GetEntities()[0];
		ASSERT_EQ(entity->GetComponents().Size(), 1u);

		Component* component = entity->GetComponents()[0];
		ASSERT_EQ(component->GetReflectionDescriptorV().GetType(), MissingComponent::GetReflectionDescriptor().GetType());
		EXPECT_EQ(static_cast<MissingComponent*>(component)->GetMissingType(), 0x1234567890ull);
		EXPECT_EQ(component->GetLocalId(), 2u);
		EXPECT_FALSE(component->GetEnabled());

		Document        saved;
		SceneSerializer saver;
		uint64_t        nextLocalId = 10;
		ASSERT_TRUE(saver.SerializeEntity(entity, true, saved.GetRootNode(), nextLocalId));

		const DocumentNode* componentNode = saved.GetRootNode().GetFirstChild()->GetChild("Components")->GetFirstChild();
		ASSERT_NE(componentNode, nullptr);
		EXPECT_EQ(componentNode->GetChild("MetaType")->GetUInt64(), 0x1234567890ull);
		EXPECT_EQ(componentNode->GetChild("_localId")->GetUInt64(), 2u);
		EXPECT_FALSE(componentNode->GetChild("_enabled")->GetBool());
		EXPECT_FLOAT_EQ(componentNode->GetChild("GravityScale")->GetFloat32(), 2.5f);
		EXPECT_EQ(componentNode->GetChild("Mode")->GetInt32(), 2);

		DefaultAllocator::GetInstance().Delete(entity);
	}
}
