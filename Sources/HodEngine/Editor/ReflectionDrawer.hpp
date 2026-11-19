#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <string_view>

namespace hod::inline core
{
    class EnumDescriptor;
    class ReflectionDescriptor;
	class ReflectionProperty;
    class ReflectionPropertyVariable;
    class ReflectionPropertyArray;
    class ReflectionPropertyObject;
}

namespace hod::inline math
{
    class Vector2;
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

		/// @brief value alone, outside of a reflected object (a material's parameter...), on the rest of the line: by the PropertyDrawer of
		/// its type, its properties otherwise. No label, override or context menu
		/// @return true if it changed
		template<typename _Type_>
		static bool     DrawValue(_Type_& value);
		static bool     DrawValue(void* value, const ReflectionDescriptor& descriptor);

        // Draws a value of an enum (e.g. an easing's shape) in a tooltip: of each entry of the enum's combos, and of a closed combo
        // for its value. Registered by the module owning the enum's editor, through its EditorRegistrar.
        using EnumValuePreview = void (*)(uint64_t value);
        // @return false (with an error) if the enum has a preview already
        static bool     RegisterEnumValuePreview(const EnumDescriptor& enumDescriptor, EnumValuePreview preview);
        static void     UnregisterEnumValuePreview(const EnumDescriptor& enumDescriptor);

        static bool     BeginProperty(EditorReflectedProperty& editorReflectedProperty);

        /// @return Whether the property is shown: not hidden by its ReflectionTraitVisibleIf for each of its owners (shown if one uses it)
        static bool     IsPropertyVisible(const EditorReflectedProperty& editorReflectedProperty);
        /// @return Whether the property is enabled: not disabled by its ReflectionTraitEnabledIf for each of its owners (enabled if one uses it)
        static bool     IsPropertyEnabled(const EditorReflectedProperty& editorReflectedProperty);

        /// @brief The property of owner highlighted (the one deciding why a disabled property hovered is, see ReflectionTraitEnabledIf):
        /// during this frame and the next one (see PropertyHighlight)
        /// @param owner The first of the objects edited together
        /// @param property Its name, as written in its class
        static void     RequestHighlight(const void* owner, std::string_view property);
        /// @return Whether the property is highlighted (DrawProperty frames its row; a row drawn by hand calls HighlightLastItem)
        static bool     IsHighlighted(const EditorReflectedProperty& editorReflectedProperty);
        /// @brief The last ImGui item framed as a highlighted property (as the navigation cursor)
        static void     HighlightLastItem();
        /// @brief A rect of the window (screen coordinates) framed as a highlighted property
        static void     HighlightRect(const Vector2& min, const Vector2& max);
        static bool     DrawContextMenu(EditorReflectedProperty& editorReflectedProperty);
    };

	/// @brief
	/// @tparam _Type_
	/// @param value
	/// @return
	template<typename _Type_>
	bool ReflectionDrawer::DrawValue(_Type_& value)
	{
		return DrawValue(&value, _Type_::GetReflectionDescriptor());
	}
}
