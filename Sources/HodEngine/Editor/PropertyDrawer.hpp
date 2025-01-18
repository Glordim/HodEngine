#pragma once
#include "HodEngine/Editor/Export.hpp"

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
        static bool     DrawProperty(EditorReflectedProperty& reflectedProperty);
		static bool     DrawPropertyVariable(EditorReflectedProperty& reflectedProperty);
		static bool     DrawPropertyArray(EditorReflectedProperty& reflectedProperty);
		static bool     DrawPropertyObject(EditorReflectedProperty& reflectedProperty);

        static bool     BeginProperty(EditorReflectedProperty& editorReflectedProperty);
    };
}
