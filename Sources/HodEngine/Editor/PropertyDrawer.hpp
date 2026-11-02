#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>

namespace hod::inline core
{
    class EnumDescriptor;
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

        // Draws a value of an enum (e.g. an easing's shape) in a tooltip: of each entry of the enum's combos, and of a closed combo
        // for its value. Registered by the module owning the enum's editor, through its EditorModuleRegistration.
        using EnumValuePreview = void (*)(uint64_t value);
        // @return false (with an error) if the enum has a preview already
        static bool     RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, EnumValuePreview preview);
        static void     UnregisterEnumValuePreview(const EnumDescriptor& enumDescriptor);

        static bool     BeginProperty(EditorReflectedProperty& editorReflectedProperty);
        static bool     DrawContextMenu(EditorReflectedProperty& editorReflectedProperty);
    };
}
