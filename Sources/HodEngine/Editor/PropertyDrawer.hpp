#pragma once

namespace hod
{
	class Object;
}

namespace hod
{
    class ReflectionDescriptor;
	class ReflectionProperty;
    class ReflectionPropertyVariable;
    class ReflectionPropertyArray;
    class ReflectionPropertyObject;
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

        static bool     DrawDescriptor(void* object, ReflectionDescriptor* descriptor);
        static bool     DrawProperty(void* object, ReflectionProperty* property);
		static bool     DrawPropertyVariable(void* object, ReflectionPropertyVariable* property);
		static bool     DrawPropertyArray(void* object, ReflectionPropertyArray* property);
		static bool     DrawPropertyObject(void* object, ReflectionPropertyObject* property);
    };
}
