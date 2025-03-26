#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

namespace hod::editor
{
    /// @brief 
    /// @param owner 
    EditorTabWindow::EditorTabWindow(EditorTab* owner)
    : Window()
    , _owner(owner)
    {
    }

    /// @brief 
    EditorTabWindow::~EditorTabWindow()
    {
    }

    /// @brief 
    /// @return 
    EditorTab* EditorTabWindow::GetOwner() const
    {
        return _owner;
    }
}
