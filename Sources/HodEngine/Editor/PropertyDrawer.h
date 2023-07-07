#pragma once

namespace hod
{
	class Object;
}

namespace hod
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

        static bool     DrawProperty(void* object, ReflectionProperty* property);
		static bool     DrawPropertyVariable(void* object, Reflection::Property::Variable* property);
		static bool     DrawPropertyArray(void* object, Reflection::Property::Array* property);
		static bool     DrawPropertyObject(void* object, Reflection::Property::Object* property);
        // static void  RegisterCustomDrawer(); //TODO
    };
}
