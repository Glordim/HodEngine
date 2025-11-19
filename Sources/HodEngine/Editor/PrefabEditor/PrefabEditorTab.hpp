#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EntityBasedTabEditor/EntityEditorTab.hpp"

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API PrefabEditorTab : public EntityEditorTab
	{
	public:
				PrefabEditorTab(std::shared_ptr<Asset> asset);
				~PrefabEditorTab() override;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
	};
}
