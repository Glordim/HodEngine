#pragma once

namespace hod
{
	class Object;
}

namespace hod::core
{
    class ReflectionProperty;

	namespace Reflection::Property
	{
		class Variable;
		class Array;
		class Object;
	}
}

namespace hod::editor
{
    class PropertyDrawer
    {
    public:

                        PropertyDrawer() = delete;
                        PropertyDrawer(const PropertyDrawer&) = delete;
                        PropertyDrawer(PropertyDrawer&&) = delete;
                        ~PropertyDrawer() = delete;

        PropertyDrawer& operator = (const PropertyDrawer&) = delete;
        PropertyDrawer& operator = (PropertyDrawer&&) = delete;

    public:

        static bool     DrawProperty(Object& object, core::ReflectionProperty* property);
		static bool     DrawPropertyVariable(Object& object, core::Reflection::Property::Variable* property);
		static bool     DrawPropertyArray(Object& object, core::Reflection::Property::Array* property);
		static bool     DrawPropertyObject(Object& object, core::Reflection::Property::Object* property);
        // static void  RegisterCustomDrawer(); //TODO
    };
}
