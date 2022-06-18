#include "Editor.h"

#include "HodEngine/Editor/src/MainBar.h"

#include <HodEngine/ImGui/src/ImGuiManager.h>

namespace hod::editor
{
	/// @brief 
	Editor::Editor()
	{
	}

	/// @brief 
	Editor::~Editor()
	{
		delete _mainBar;
	}

	/// @brief 
	/// @param window 
	/// @return 
	bool Editor::Init()
	{
		_mainBar = new MainBar();

		imgui::ImGuiManager::GetInstance()->SetMainBar(_mainBar);

		return true;
	}
}
