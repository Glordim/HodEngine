#include <gtest/gtest.h>

#include <HodEngine/Editor/EditorReflectedObject.hpp>
#include <HodEngine/Editor/EditorReflectedProperty.hpp>
#include <HodEngine/Editor/DrawHelper.hpp>
#include <HodEngine/Editor/PropertyDrawer/PropertyValues.hpp>
#include <HodEngine/Editor/PropertyHighlight.hpp>
#include <HodEngine/Editor/ReflectionDrawer.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitVectorComponentEnabledIf.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitVectorComponentLabels.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitVisibleIf.hpp>
#include <HodEngine/Math/Vector2.hpp>

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

	/// @brief A size whose width is used only in a mode
	class VectorTraitsOwner
	{
		REFLECTED_CLASS_NO_VIRTUAL(VectorTraitsOwner)

	public:
		bool IsWidthUsed() const
		{
			return _widthUsed;
		}

		bool    _widthUsed = false;
		Vector2 _size;
		Vector2 _plain;
	};

	DESCRIBE_REFLECTED_CLASS(VectorTraitsOwner)
	{
		ReflectionProperty* sizeProperty = HOD_PROPERTY(_size);
		sizeProperty->AddTrait<ReflectionTraitVectorComponentLabels>("W", "H");
		sizeProperty->AddTrait<ReflectionTraitVectorComponentEnabledIf>(&VectorTraitsOwner::IsWidthUsed, "Unused: no width", "_widthUsed", nullptr, "", ""); // The height always
		HOD_PROPERTY(_widthUsed);
		HOD_PROPERTY(_plain);
	}

	// The labels and the components unused from the traits; a component enabled if one owner uses it; nothing without traits or property
	TEST(ReflectionDrawer, VectorTraits)
	{
		VectorTraitsOwner first;
		VectorTraitsOwner second;

		EditorReflectedObject object(Vector<void*>({&first, &second}), &VectorTraitsOwner::GetReflectionDescriptor(), nullptr, nullptr);
		{
			PropertyValuesBase values(*object.FindProperty("_size"));
			VectorFieldOptions options;
			DrawHelper::ApplyVectorTraits(values, 2, options);
			EXPECT_STREQ(options._labels[0], "W");
			EXPECT_STREQ(options._labels[1], "H");
			EXPECT_EQ(options._disabledComponents, 1); // The width
			EXPECT_STREQ(options._disabledReasons[0], "Unused: no width");
			EXPECT_EQ(options._disabledReasons[1], nullptr);
			EXPECT_STREQ(options._decidingProperties[0], "_widthUsed");
			EXPECT_EQ(options._owner, &first);
		}

		second._widthUsed = true;
		{
			PropertyValuesBase values(*object.FindProperty("_size"));
			VectorFieldOptions options;
			DrawHelper::ApplyVectorTraits(values, 2, options);
			EXPECT_EQ(options._disabledComponents, 0);
			EXPECT_EQ(options._disabledReasons[0], nullptr);
		}

		{
			PropertyValuesBase values(*object.FindProperty("_plain"));
			VectorFieldOptions options;
			DrawHelper::ApplyVectorTraits(values, 2, options);
			EXPECT_STREQ(options._labels[0], "X");
			EXPECT_EQ(options._disabledComponents, 0);
		}

		{
			Vector2            value;
			PropertyValuesBase values(&value);
			VectorFieldOptions options;
			DrawHelper::ApplyVectorTraits(values, 2, options);
			EXPECT_STREQ(options._labels[1], "Y");
		}
	}

	// A request lasts its frame and the next one, for its owner only
	TEST(ReflectionDrawer, PropertyHighlight)
	{
		int owner = 0;
		int other = 0;

		PropertyHighlight highlight;
		highlight.Request(&owner, "_mode");
		EXPECT_TRUE(highlight.IsHighlighted(&owner, "_mode")); // Drawn after the property hovered
		EXPECT_FALSE(highlight.IsHighlighted(&other, "_mode"));
		EXPECT_FALSE(highlight.IsHighlighted(&owner, "_size"));

		highlight.NewFrame();
		EXPECT_TRUE(highlight.IsHighlighted(&owner, "_mode")); // Drawn before it

		highlight.NewFrame();
		EXPECT_FALSE(highlight.IsHighlighted(&owner, "_mode")); // Not hovered anymore

		highlight.Request(&owner, "");
		highlight.Request(nullptr, "_mode");
		EXPECT_FALSE(highlight.IsHighlighted(nullptr, "_mode"));
	}
}
