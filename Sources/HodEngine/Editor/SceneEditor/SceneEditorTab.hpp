#pragma once
#include "HodEngine/Editor/EditorTab.hpp"

namespace hod::editor
{
	/// @brief 
	class SceneEditorTab : public EditorTab
	{
	public:
		SceneEditorTab();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
	};
}
