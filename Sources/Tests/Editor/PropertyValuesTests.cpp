#include <gtest/gtest.h>

#include <HodEngine/Editor/EditorReflectedObject.hpp>
#include <HodEngine/Editor/EditorReflectedProperty.hpp>
#include <HodEngine/Editor/PropertyDrawer/PropertyValues.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Math/Vector2.hpp>

namespace hod
{
	/// @brief An owner of a Vector2 with a set method (counting its calls) and of one without
	class PropertyValuesTestOwner
	{
		REFLECTED_CLASS_NO_VIRTUAL(PropertyValuesTestOwner)

	public:
		void SetWithSetter(const Vector2& value)
		{
			_withSetter = value;
			++_setterCalls;
		}

		Vector2  _withSetter;
		Vector2  _withoutSetter;
		uint32_t _setterCalls = 0;
	};

	DESCRIBE_REFLECTED_CLASS(PropertyValuesTestOwner)
	{
		HOD_PROPERTY(_withSetter, &PropertyValuesTestOwner::SetWithSetter);
		HOD_PROPERTY(_withoutSetter);
	}

	// One value (ReflectionDrawer::DrawValue): never mixed, edited in place
	TEST(PropertyValues, OneValue)
	{
		Vector2                  value(1.0f, 2.0f);
		PropertyValuesBase       base(&value);
		PropertyValues<Vector2> values(base);

		EXPECT_EQ(values.GetCount(), 1u);
		EXPECT_FALSE(values.IsMixed());
		EXPECT_FALSE(values.IsMixed([](const Vector2& vector) { return vector.GetX(); }));

		values.Edit([](Vector2& edited) { edited.SetY(5.0f); });
		EXPECT_FLOAT_EQ(value.GetX(), 1.0f);
		EXPECT_FLOAT_EQ(value.GetY(), 5.0f);

		values.Set(Vector2(3.0f, 4.0f));
		EXPECT_FLOAT_EQ(value.GetX(), 3.0f);
		EXPECT_FLOAT_EQ(value.GetY(), 4.0f);
	}

	// A property of several instances (a multi-selection): mixed as a whole and per part, each edited on its own, through its set method
	TEST(PropertyValues, PropertyOfSeveralInstances)
	{
		PropertyValuesTestOwner first;
		PropertyValuesTestOwner second;
		first._withSetter = Vector2(1.0f, 2.0f);
		second._withSetter = Vector2(1.0f, 7.0f);

		EditorReflectedObject    object(Vector<void*>({&first, &second}), &PropertyValuesTestOwner::GetReflectionDescriptor(), nullptr, nullptr);
		EditorReflectedProperty* property = object.FindProperty("_withSetter");
		ASSERT_NE(property, nullptr);

		PropertyValuesBase       base(*property);
		PropertyValues<Vector2> values(base);

		EXPECT_EQ(values.GetCount(), 2u);
		EXPECT_FLOAT_EQ(values.Get().GetY(), 2.0f); // The first one's
		EXPECT_TRUE(values.IsMixed());
		EXPECT_FALSE(values.IsMixed([](const Vector2& vector) { return vector.GetX(); }));
		EXPECT_TRUE(values.IsMixed([](const Vector2& vector) { return vector.GetY(); }));

		// X only: each keeps its Y
		values.Edit([](Vector2& edited) { edited.SetX(9.0f); });
		EXPECT_FLOAT_EQ(first._withSetter.GetX(), 9.0f);
		EXPECT_FLOAT_EQ(first._withSetter.GetY(), 2.0f);
		EXPECT_FLOAT_EQ(second._withSetter.GetX(), 9.0f);
		EXPECT_FLOAT_EQ(second._withSetter.GetY(), 7.0f);
		EXPECT_EQ(first._setterCalls, 1u);
		EXPECT_EQ(second._setterCalls, 1u);
	}

	// Without a set method: edited in place
	TEST(PropertyValues, PropertyWithoutSetter)
	{
		PropertyValuesTestOwner owner;

		EditorReflectedObject    object(&owner, &PropertyValuesTestOwner::GetReflectionDescriptor(), nullptr, nullptr);
		EditorReflectedProperty* property = object.FindProperty("_withoutSetter");
		ASSERT_NE(property, nullptr);

		PropertyValuesBase       base(*property);
		PropertyValues<Vector2> values(base);
		EXPECT_FALSE(values.IsMixed());

		values.Set(Vector2(4.0f, 5.0f));
		EXPECT_FLOAT_EQ(owner._withoutSetter.GetX(), 4.0f);
		EXPECT_FLOAT_EQ(owner._withoutSetter.GetY(), 5.0f);
		EXPECT_EQ(owner._setterCalls, 0u);
	}
}
