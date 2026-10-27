#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"

namespace hod::inline editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API PrefabEditorTab : public EntityEditorTab
	{
	public:
				PrefabEditorTab(std::shared_ptr<Asset> asset);
				~PrefabEditorTab() override;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		bool	OnSave() override;
	};
}
