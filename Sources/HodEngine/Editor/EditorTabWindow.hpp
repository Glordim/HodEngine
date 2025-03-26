#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::editor
{
    class EditorTab;

    /// @brief 
    class HOD_EDITOR_API EditorTabWindow : public imgui::Window
    {
    public:
        EditorTabWindow(EditorTab* owner = nullptr); // todo remove nullptr default value

        template<typename _EditorTabType_>
        _EditorTabType_*    GetOwner() const;
        EditorTab*          GetOwner() const;

    private:
        EditorTab* _owner;
    };

    /// @brief 
    /// @tparam _EditorTabType_ 
    /// @return 
    template<typename _EditorTabType_>
    _EditorTabType_* EditorTabWindow::GetOwner() const
    {
        return static_cast<_EditorTabType_*>(GetOwner());
    }
}
