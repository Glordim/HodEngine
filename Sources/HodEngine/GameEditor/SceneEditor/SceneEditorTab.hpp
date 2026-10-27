#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include <memory>

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API SceneEditorTab : public EntityEditorTab
	{
	public:
				SceneEditorTab(std::shared_ptr<Asset> asset);
				~SceneEditorTab() override;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		bool	OnSave() override;
	};
}
