#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

namespace hod::editor
{
    DESCRIBE_REFLECTED_CLASS(EditorTabWindow, reflectionDescriptor)
    {
        (void)reflectionDescriptor;
    }

    /// @brief 
    /// @param owner 
    EditorTabWindow::EditorTabWindow(EditorTab* owner)
    : Window()
    , _owner(owner)
    {
        SetId(reinterpret_cast<uint64_t>(owner));
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
