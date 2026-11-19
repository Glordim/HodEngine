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

    /// @brief Draws what reflection describes, for the inspector, the asset tabs, the project settings...: an object's properties
    /// (DrawDescriptor), each one by its kind (variable, array, object), a value of a type having a PropertyDrawer<T> with it (DrawProperty),
    /// of an object type without one under a collapsing header. Also the parts the property drawers use: a property's label and context menu
    /// (BeginProperty), the enums' previews
    class HOD_EDITOR_API ReflectionDrawer
    {
    public:

                        ReflectionDrawer() = delete;
                        ReflectionDrawer(const ReflectionDrawer&) = delete;
                        ReflectionDrawer(ReflectionDrawer&&) = delete;
                        ~ReflectionDrawer() = delete;

        ReflectionDrawer& operator = (const ReflectionDrawer&) = delete;
        ReflectionDrawer& operator = (ReflectionDrawer&&) = delete;

    public:

        static bool     DrawDescriptor(EditorReflectedObject& reflectedObject);
        static bool     DrawProperty(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyVariable(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyArray(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);
		static bool     DrawPropertyObject(EditorReflectedProperty& reflectedProperty, bool onlyValue = false);

        // Draws a value of an enum (e.g. an easing's shape) in a tooltip: of each entry of the enum's combos, and of a closed combo
        // for its value. Registered by the module owning the enum's editor, through its EditorRegistrar.
        using EnumValuePreview = void (*)(uint64_t value);
        // @return false (with an error) if the enum has a preview already
        static bool     RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, EnumValuePreview preview);
        static void     UnregisterEnumValuePreview(const EnumDescriptor& enumDescriptor);

        static bool     BeginProperty(EditorReflectedProperty& editorReflectedProperty);
        static bool     DrawContextMenu(EditorReflectedProperty& editorReflectedProperty);
    };
}
