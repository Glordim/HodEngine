#pragma once
#include "HodEngine/Editor/Export.hpp"

namespace hod::inline core
{
    class ReflectionDescriptor;
	class ReflectionProperty;
    class ReflectionPropertyVariable;
    class ReflectionPropertyArray;
    class ReflectionPropertyObject;
}

namespace hod::inline editor
{
    class EditorReflectedObject;
    class EditorReflectedProperty;

    class HOD_EDITOR_API PropertyDrawer
    {
    public:

                        PropertyDrawer() = delete;
                        PropertyDrawer(const PropertyDrawer&) = delete;
                        PropertyDrawer(PropertyDrawer&&) = delete;
                        ~PropertyDrawer() = delete;

        PropertyDrawer& operator = (const PropertyDrawer&) = delete;
        PropertyDrawer& operator = (PropertyDrawer&&) = delete;

    public:

        static bool     DrawDescriptor(EditorReflectedObject& reflectedObject);
        static bool     DrawProperty(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyVariable(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyArray(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyObject(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);

        static bool     BeginProperty(EditorReflectedProperty& editorReflectedProperty);
    };
}
