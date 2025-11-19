#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EntityBasedTabEditor/EntityEditorTab.hpp"

#include <memory>

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API SceneEditorTab : public EntityEditorTab
	{
	public:
				SceneEditorTab(std::shared_ptr<Asset> asset);
				~SceneEditorTab() override;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
	};
}
