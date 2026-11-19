#include <gtest/gtest.h>

#include <HodEngine/Editor/EditorReflectedObject.hpp>
#include <HodEngine/Editor/EditorReflectedProperty.hpp>
#include <HodEngine/Editor/ReflectionDrawer.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitVisibleIf.hpp>

namespace hod
{
	/// @brief _spacing used only when spaced, _curve only with a custom easing
	class ConditionalPropertiesOwner
	{
		REFLECTED_CLASS_NO_VIRTUAL(ConditionalPropertiesOwner)

	public:
		bool IsSpaced() const
		{
			return _spaced;
		}

		bool HasCustomEasing() const
		{
			return _customEasing;
		}

		bool  _spaced = false;
		bool  _customEasing = false;
		float _spacing = 0.0f;
		float _curve = 0.0f;
		float _always = 0.0f;
	};

	DESCRIBE_REFLECTED_CLASS(ConditionalPropertiesOwner)
	{
		HOD_PROPERTY(_spaced);
		HOD_PROPERTY(_customEasing);
		HOD_PROPERTY(_spacing)->AddTrait<ReflectionTraitEnabledIf>(&ConditionalPropertiesOwner::IsSpaced, "Unused: not spaced");
		HOD_PROPERTY(_curve)->AddTrait<ReflectionTraitVisibleIf>(&ConditionalPropertiesOwner::HasCustomEasing);
		HOD_PROPERTY(_always);
	}

	TEST(ReflectionDrawer, TraitsAskTheOwner)
	{
		ConditionalPropertiesOwner owner;
		ReflectionDescriptor&      descriptor = ConditionalPropertiesOwner::GetReflectionDescriptor();

		ReflectionTraitEnabledIf* enabledIf = descriptor.FindProperty("_spacing")->FindTrait<ReflectionTraitEnabledIf>();
		ASSERT_NE(enabledIf, nullptr);
		EXPECT_EQ(enabledIf->GetReason(), "Unused: not spaced");
		EXPECT_FALSE(enabledIf->IsEnabled(&owner));
		owner._spaced = true;
		EXPECT_TRUE(enabledIf->IsEnabled(&owner));

		ReflectionTraitVisibleIf* visibleIf = descriptor.FindProperty("_curve")->FindTrait<ReflectionTraitVisibleIf>();
		ASSERT_NE(visibleIf, nullptr);
		EXPECT_FALSE(visibleIf->IsVisible(&owner));
		owner._customEasing = true;
		EXPECT_TRUE(visibleIf->IsVisible(&owner));
	}

	// Several owners edited together: enabled / shown if one of them uses it; a property without the trait always is
	TEST(ReflectionDrawer, EnabledAndVisibleIfOneOwnerUsesIt)
	{
		ConditionalPropertiesOwner first;
		ConditionalPropertiesOwner second;

		EditorReflectedObject object(Vector<void*>({&first, &second}), &ConditionalPropertiesOwner::GetReflectionDescriptor(), nullptr, nullptr);
		EditorReflectedProperty* spacing = object.FindProperty("_spacing");
		EditorReflectedProperty* curve = object.FindProperty("_curve");
		EditorReflectedProperty* always = object.FindProperty("_always");
		ASSERT_NE(spacing, nullptr);
		ASSERT_NE(curve, nullptr);
		ASSERT_NE(always, nullptr);

		EXPECT_FALSE(ReflectionDrawer::IsPropertyEnabled(*spacing));
		EXPECT_FALSE(ReflectionDrawer::IsPropertyVisible(*curve));
		EXPECT_TRUE(ReflectionDrawer::IsPropertyEnabled(*always));
		EXPECT_TRUE(ReflectionDrawer::IsPropertyVisible(*always));

		second._spaced = true;
		second._customEasing = true;
		EXPECT_TRUE(ReflectionDrawer::IsPropertyEnabled(*spacing));
		EXPECT_TRUE(ReflectionDrawer::IsPropertyVisible(*curve));
	}
}
