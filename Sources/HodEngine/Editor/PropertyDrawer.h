#pragma once

namespace hod::core
{
    class ReflectionProperty;

	namespace Reflection::Property
	{
		class Variable;
		class Array;
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

        static void     DrawProperty(core::ReflectionProperty* property);
		static void     DrawPropertyVariable(core::Reflection::Property::Variable* property);
		static void     DrawPropertyArray(core::Reflection::Property::Array* property);
        // static void  RegisterCustomDrawer(); //TODO
    };
}
