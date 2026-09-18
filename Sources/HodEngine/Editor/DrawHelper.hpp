#pragma once
#include "HodEngine/Editor/Export.hpp"

namespace hod::inline math
{
    class Vector2;
    class Vector4;
}

namespace hod::inline editor
{
    class HOD_EDITOR_API DrawHelper
    {
    public:

        static bool				DrawVector2(Vector2& value, const char* labelX = "X", const char* labelY = "Y");
        static bool				DrawVector4(Vector4& value, const char* labelX = "X", const char* labelY = "Y", const char* labelZ = "Z", const char* labelW = "W");

        /// @brief Draws the framed, collapsible block used by inspectors to compartment their content
        /// (one per component, per node/layout/layout params...): a header row (collapse arrow, optional
        /// enable checkbox, icon, title, optional close button revealed when the mouse gets close),
        /// followed by the block's content, which the caller draws only when this returns true.
        /// Every call must be paired with EndInspectorBlock(), whatever it returns.
        /// @param id unique identifier of the block among its siblings (e.g. the object address).
        /// @param enabled when not null, shows an enable checkbox bound to it (the title is greyed out when disabled).
        /// @param removeRequested when not null, shows a close button which sets it to true when clicked.
        /// @return true if the block is expanded, i.e. its content must be drawn
        static bool				BeginInspectorBlock(const void* id, const char* icon, const char* title, bool* enabled = nullptr, bool* removeRequested = nullptr);
        static void				EndInspectorBlock();
    };
}
