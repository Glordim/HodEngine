#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/AnimationClip.hpp>
#include <HodEngine/UI2/AnimationPlayer.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>
#include <HodEngine/UI2/LayoutParamsFactory.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

#include <map>
#include <memory>
#include <vector>

namespace hod::animation_tests // named: the other test files have helpers of the same names (unity builds)
{
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

		Node* NewNode(const char* name)
		{
			Node* node = DefaultAllocator::GetInstance().New<Node>();
			node->SetName(name);
			return node;
		}

		PropertyValue RotationValue(float rotation)
		{
			Node scratch;
			scratch.SetRotation(rotation);
			PropertyValue value;
			value.Read(scratch, "Rotation");
			return value;
		}

		PropertyValue ScaleValue(const Vector2& scale)
		{
			Node scratch;
			scratch.SetScale(scale);
			PropertyValue value;
			value.Read(scratch, "Scale");
			return value;
		}

		// "Spin": target's Rotation from 0 to 10 over a second
		AnimationClip MakeSpin(uint64_t target, AnimationClip::WrapMode wrapMode = AnimationClip::WrapMode::Once, AnimationKey::Easing easing = AnimationKey::Easing::Linear)
		{
			AnimationClip clip;
			clip._name = "Spin";
			clip._duration = 1.0f;
			clip._wrapMode = wrapMode;
			clip.SetKey(target, "Rotation", 0.0f, RotationValue(0.0f));
			clip.SetKey(target, "Rotation", 1.0f, RotationValue(10.0f));
			clip._tracks[0]._keys[0]._easing = easing;
			return clip;
		}
	}

	class AnimationTest : public ::testing::Test
	{
	protected:
		static void SetUpTestSuite()
		{
			NodeFactory::CreateInstance()->Register<Node>();
			LayoutParamsFactory::CreateInstance()->Register<AnchoredLayoutParams>();
			LayoutFactory::CreateInstance();
			InteractionFactory::CreateInstance();
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

			// As a canvas holds a prefab: container 1, root 2, label 3
			_root = NewNode("Root");
			_label = NewNode("Label");
			_container.AddChild(_root);
			_root->AddChild(_label);
			uint64_t nextLocalId = 1;
			_container.AssignLocalIds(nextLocalId);
		}

		void TearDown() override
		{
			PrefabInstance::SetSourceProvider(nullptr);
			prefabDocuments.clear();
		}

		// Saves root as a prefab (ids as in SetUp)
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

		Node  _container;
		Node* _root = nullptr;
		Node* _label = nullptr;
	};

	TEST_F(AnimationTest, PlaysThenHoldsTheLastValues)
	{
		_root->GetAnimations().PushBack(MakeSpin(3));
		AnimationPlayer& player = _root->GetAnimationPlayer();

		std::vector<String> finished;
		player.GetFinishedEvent().Add([&finished](const String& name) { finished.push_back(name); });

		ASSERT_TRUE(player.Play("Spin"));
		EXPECT_FALSE(player.Play("Missing"));
		EXPECT_TRUE(player.IsPlaying("Spin"));
		EXPECT_FLOAT_EQ(_label->GetRotation(), 0.0f);

		_root->UpdateAnimations(0.5f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 5.0f);
		EXPECT_TRUE(finished.empty());

		_root->UpdateAnimations(0.6f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 10.0f);
		EXPECT_FALSE(player.IsPlaying("Spin"));
		ASSERT_EQ(finished.size(), 1u);
		EXPECT_TRUE(finished[0] == "Spin");

		_root->UpdateAnimations(0.5f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 10.0f);
	}

	TEST_F(AnimationTest, KeyEasingShapesTheSegment)
	{
		_root->GetAnimations().PushBack(MakeSpin(3, AnimationClip::WrapMode::Once, AnimationKey::Easing::Constant));
		_root->GetAnimationPlayer().Play("Spin");
		_root->UpdateAnimations(0.9f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 0.0f);

		_root->GetAnimations()[0]._tracks[0]._keys[0]._easing = AnimationKey::Easing::QuadIn;
		_root->GetAnimationPlayer().Play("Spin"); // resolved again
		_root->UpdateAnimations(0.5f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 2.5f);

		EXPECT_FLOAT_EQ(AnimationKey::Ease(AnimationKey::Easing::BounceOut, 1.0f), 1.0f);
		EXPECT_FLOAT_EQ(AnimationKey::Ease(AnimationKey::Easing::ElasticOut, 1.0f), 1.0f);
		EXPECT_NEAR(AnimationKey::Ease(AnimationKey::Easing::BackInOut, 1.0f), 1.0f, 1e-5f);
		EXPECT_GT(AnimationKey::Ease(AnimationKey::Easing::BackOut, 0.5f), 1.0f); // overshoots
	}

	TEST_F(AnimationTest, LoopAndPingPongWrapTheTime)
	{
		_root->GetAnimations().PushBack(MakeSpin(3, AnimationClip::WrapMode::Loop));
		_root->GetAnimationPlayer().Play("Spin");
		_root->UpdateAnimations(1.25f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 2.5f);
		EXPECT_TRUE(_root->GetAnimationPlayer().IsPlaying("Spin"));

		_root->GetAnimations()[0]._wrapMode = AnimationClip::WrapMode::PingPong;
		_root->GetAnimationPlayer().Play("Spin");
		_root->UpdateAnimations(1.25f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 7.5f);
		_root->UpdateAnimations(1.0f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 2.5f);
	}

	TEST_F(AnimationTest, NegativeSpeedPlaysBackwards)
	{
		_root->GetAnimations().PushBack(MakeSpin(3));
		AnimationPlayer& player = _root->GetAnimationPlayer();
		int              finishedCount = 0;
		player.GetFinishedEvent().Add([&finishedCount](const String&) { ++finishedCount; });

		player.Play("Spin", -2.0f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 10.0f);
		_root->UpdateAnimations(0.125f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 7.5f);
		_root->UpdateAnimations(1.0f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 0.0f);
		EXPECT_EQ(finishedCount, 1);
	}

	TEST_F(AnimationTest, StopPutsBackTheBaseValues)
	{
		_label->SetRotation(3.0f);
		_root->GetAnimations().PushBack(MakeSpin(3));
		AnimationPlayer& player = _root->GetAnimationPlayer();

		player.Play("Spin");
		_root->UpdateAnimations(0.5f);
		player.Stop("Spin", false);
		EXPECT_FALSE(player.IsPlaying("Spin"));
		EXPECT_FLOAT_EQ(_label->GetRotation(), 5.0f);

		// Also once it finished, back to what the prefab gave before any clip
		player.Play("Spin");
		_root->UpdateAnimations(2.0f);
		EXPECT_FLOAT_EQ(_label->GetRotation(), 10.0f);
		player.Stop("Spin");
		EXPECT_FLOAT_EQ(_label->GetRotation(), 3.0f);

		// Given back: a later change is the new base
		_label->SetRotation(4.0f);
		player.Play("Spin");
		player.StopAll();
		EXPECT_FLOAT_EQ(_label->GetRotation(), 4.0f);
	}

	TEST_F(AnimationTest, ObjectPropertiesEaseMemberwise)
	{
		AnimationClip clip;
		clip._name = "Grow";
		clip.SetKey(3, "Scale", 0.0f, ScaleValue(Vector2(1.0f, 1.0f)));
		clip.SetKey(3, "Scale", 1.0f, ScaleValue(Vector2(3.0f, 5.0f)));
		clip.SetKey(3, "Scale", 1.0f, ScaleValue(Vector2(3.0f, 9.0f))); // replaces
		ASSERT_EQ(clip._tracks.Size(), 1u);
		ASSERT_EQ(clip._tracks[0]._keys.Size(), 2u);
		_root->GetAnimations().PushBack(clip);

		ASSERT_TRUE(_root->GetAnimationPlayer().Sample("Grow", 0.5f));
		EXPECT_FLOAT_EQ(_label->GetScale().GetX(), 2.0f);
		EXPECT_FLOAT_EQ(_label->GetScale().GetY(), 5.0f);
	}

	TEST_F(AnimationTest, ClipsRoundTripAndFollowCopiedIds)
	{
		_root->GetAnimations().PushBack(MakeSpin(3, AnimationClip::WrapMode::PingPong, AnimationKey::Easing::CubicOut));
		_root->GetAnimations()[0]._duration = 2.0f;

		Document document;
		ASSERT_TRUE(_root->SerializeAsChildInDocument(document.GetRootNode()));
		// The nodes without clips write nothing for them
		const DocumentNode* labelEntry = document.GetRootNode().GetChild("Children")->GetFirstChild();
		EXPECT_EQ(labelEntry->GetChild("Animations"), nullptr);

		Node  other;
		Node* copy = other.AddChildFromDocument(document.GetRootNode());
		ASSERT_NE(copy, nullptr);
		ASSERT_EQ(copy->GetAnimations().Size(), 1u);
		const AnimationClip& clip = copy->GetAnimations()[0];
		EXPECT_TRUE(clip._name == "Spin");
		EXPECT_FLOAT_EQ(clip._duration, 2.0f);
		EXPECT_EQ(clip._wrapMode, AnimationClip::WrapMode::PingPong);
		ASSERT_EQ(clip._tracks.Size(), 1u);
		ASSERT_EQ(clip._tracks[0]._keys.Size(), 2u);
		EXPECT_EQ(clip._tracks[0]._keys[0]._easing, AnimationKey::Easing::CubicOut);
		EXPECT_NE(copy->FindAnimation("Spin"), nullptr);
		EXPECT_EQ(copy->FindAnimation("Other"), nullptr);

		// A copy pasted elsewhere: the label gets a new id, the track follows it
		uint64_t nextLocalId = 10;
		copy->RenewLocalIds(nextLocalId, false);
		Node* copiedLabel = copy->GetChildren()[0];
		EXPECT_EQ(copy->GetAnimations()[0]._tracks[0]._target, copiedLabel->GetLocalId());

		copy->GetAnimationPlayer().Sample("Spin", 1.0f);
		EXPECT_FLOAT_EQ(copiedLabel->GetRotation(), 10.0f);

		// Convert To keeps them
		Node* replacement = NewNode("Replacement");
		copy->ReplaceWith(replacement);
		DefaultAllocator::GetInstance().Delete(copy);
		ASSERT_EQ(replacement->GetAnimations().Size(), 1u);
		replacement->GetAnimationPlayer().Sample("Spin", 0.0f);
		EXPECT_FLOAT_EQ(copiedLabel->GetRotation(), 0.0f);
	}

	TEST_F(AnimationTest, InstancePlaysItsPrefabsClipsInItsScope)
	{
		Node* prefabRoot = NewNode("Root"); // id 2
		Node* prefabLabel = NewNode("Label"); // id 3
		prefabRoot->AddChild(prefabLabel);
		prefabRoot->GetAnimations().PushBack(MakeSpin(3));
		UID prefab = AddPrefab(prefabRoot);

		// The containing prefab has a node of id 3 too: outer 1, instance 2, decoy 3
		Node     outer;
		Document entry;
		entry.GetRootNode().AddChild("PrefabInstance").AddChild("Prefab").SetString(prefab.ToString());
		Node* instance = outer.AddChildFromDocument(entry.GetRootNode());
		ASSERT_NE(instance, nullptr);
		Node* decoy = NewNode("Decoy");
		outer.AddChild(decoy);
		uint64_t nextLocalId = 1;
		outer.AssignLocalIds(nextLocalId);
		ASSERT_EQ(decoy->GetLocalId(), 3u);

		ASSERT_EQ(instance->GetAnimations().Size(), 1u);
		ASSERT_TRUE(instance->GetAnimationPlayer().Play("Spin"));
		instance->UpdateAnimations(0.5f);
		EXPECT_FLOAT_EQ(instance->GetChildren()[0]->GetRotation(), 5.0f);
		EXPECT_FLOAT_EQ(decoy->GetRotation(), 0.0f);

		// Only the placement and overrides are saved for an instance, not its prefab's clips
		Document saved;
		ASSERT_TRUE(instance->SerializeAsChildInDocument(saved.GetRootNode()));
		EXPECT_EQ(saved.GetRootNode().GetChild("Animations"), nullptr);

		// Unpacked, the clips become the containing prefab's, following the new ids
		instance->UnpackPrefabInstance(nextLocalId);
		uint64_t unpackedLabelId = instance->GetChildren()[0]->GetLocalId();
		EXPECT_NE(unpackedLabelId, 3u);
		EXPECT_EQ(instance->GetAnimations()[0]._tracks[0]._target, unpackedLabelId);
	}
}
