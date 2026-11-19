#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>

namespace hod::inline core
{
    class ReflectionDescriptor;
}

namespace hod::inline math
{
    class Vector2;
    class Vector4;
}

namespace hod::inline editor
{
    class PropertyValuesBase;

    /// @brief How DrawHelper::DrawVector2/DrawVector4 show the components (one bit per component in the masks, X first)
    struct HOD_EDITOR_API VectorFieldOptions
    {
        const char* _labels[4] = {"X", "Y", "Z", "W"};
        uint8_t     _disabledComponents = 0; // Greyed out (e.g. an axis the value is unused on)
        uint8_t     _mixedComponents = 0;    // The values edited together differ on them: shown as a mixed value
        const char* _disabledReasons[4] = {}; // Why a disabled component is: its tooltip (none if nullptr)
        const char* _decidingProperties[4] = {}; // The property of _owner deciding it, highlighted while it is hovered (see ReflectionDrawer::RequestHighlight)
        const void* _owner = nullptr;
    };

    class HOD_EDITOR_API DrawHelper
    {
    public:

        // editedComponents (optional): set to the bits of the components edited, for the caller to apply only those to each value
        static bool				DrawVector2(Vector2& value, const VectorFieldOptions& options = {}, uint8_t* editedComponents = nullptr);
        static bool				DrawVector4(Vector4& value, const VectorFieldOptions& options = {}, uint8_t* editedComponents = nullptr);
        /// @brief options' labels, disabled components, their reasons and the properties deciding them from the traits of the property values
        /// are of (ReflectionTraitVectorComponentLabels, ReflectionTraitVectorComponentEnabledIf: a component enabled if one of their owners uses
        /// it); unchanged for a value of its own
        static void				ApplyVectorTraits(const PropertyValuesBase& values, uint32_t componentCount, VectorFieldOptions& options);

        /// @brief Draws a toggle button glued to a small arrow button, in one unified round rect (same look as DrawVector2):
        /// the left part switches `toggled` on/off (highlighted while on), the right one opens the popup `settingsPopupId`
        /// (highlighted while that popup is open). The caller draws the popup content between BeginSettingsPopup(settingsPopupId) and ImGui::EndPopup().
        /// @param itemName what is shown/hidden, used to build the tooltips
        /// @return true if `toggled` changed
        static bool				DrawToggleWithSettings(const char* label, const char* itemName, const char* settingsPopupId, bool& toggled);

        /// @brief Same toggle button as DrawToggleWithSettings, without the settings arrow.
        /// @param itemName what is shown/hidden, used to build the tooltip
        /// @return true if `toggled` changed
        static bool				DrawToggle(const char* label, const char* itemName, bool& toggled);

        /// @brief Same framed look as DrawToggle, for a button that only opens the popup `popupId` (a "menu down" arrow follows the label,
        /// highlighted while the popup is open). The caller draws the popup content between BeginSettingsPopup(popupId) and ImGui::EndPopup().
        /// @param tooltip optional, shown when hovering the button while the popup is closed
        static void				DrawDropDownButton(const char* label, const char* tooltip, const char* popupId);

        /// @brief ImGui::BeginPopup for a popup opened by DrawToggleWithSettings or DrawDropDownButton, anchored under it (popup's top-left corner on the widget's bottom-left corner).
        /// Must be called in the same window and the same frame as the matching DrawToggleWithSettings/DrawDropDownButton, after it.
        static bool				BeginSettingsPopup(const char* settingsPopupId);

        /// @brief Draws the framed, collapsible block used by inspectors to compartment their content
        /// (one per component, per node/layout/layout params...): a header row (collapse arrow, optional
        /// enable checkbox, icon, title, optional close button revealed when the mouse gets close),
        /// followed by the block's content, which the caller draws only when this returns true.
        /// Every call must be paired with EndInspectorBlock(), whatever it returns.
        /// @param id unique identifier of the block among its siblings (e.g. the object address).
        /// @param enabled when not null, shows an enable checkbox bound to it (the title is greyed out when disabled).
        /// @param removeRequested when not null, shows a close button which sets it to true when clicked.
        /// @return true if the block is expanded, i.e. its content must be drawn
        /// @brief Width of the small triangle ImGui draws in the top-left corner of a docked window whose tab bar is hidden
        /// (clicking it shows the tab bar back), 0 when the current window has none.
        /// Windows without padding (viewports) shift their first widgets by it so they don't cover the triangle.
        static float			GetHiddenTabBarButtonWidth();

        /// @brief Draws again that triangle (same look, the click is still handled by ImGui::Begin), on top of what the window drew:
        /// ImGui draws it in Begin, so a viewport image covering the whole window hides it. Call it after the window content, before ImGui::End.
        static void				DrawHiddenTabBarButton();

        /// @param actionIcon/actionRequested when both not null, shows an extra button (same look as the close button, just before it)
        /// which sets actionRequested to true when clicked, e.g. for the caller to open a popup. actionTooltip (optional) is shown when hovering it.
        /// @param titleTooltip (optional) shown when the icon or the title stays hovered a while (longer than the usual tooltips: the
        /// header is crossed all the time), e.g. what the block's type is for
        /// @param removeDisabledReason (optional) the remove button is shown disabled, with it as its tooltip
        static bool				BeginInspectorBlock(const void* id, const char* icon, const char* title, bool* enabled = nullptr, bool* removeRequested = nullptr,
                                                    const char* actionIcon = nullptr, const char* actionTooltip = nullptr, bool* actionRequested = nullptr,
                                                    const char* titleTooltip = nullptr, const char* removeDisabledReason = nullptr);
        static void				EndInspectorBlock();

        /// @brief Icon of a reflected type: its ReflectionTraitIcon (possibly inherited from a parent type), or fallback when it has none.
        static const char*		GetTypeIcon(const ReflectionDescriptor& descriptor, const char* fallback = "");
    };
}
