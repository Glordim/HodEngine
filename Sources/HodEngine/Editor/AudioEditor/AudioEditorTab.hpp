#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>

namespace hod::inline editor
{
	/// @brief 
	class HOD_EDITOR_API AudioEditorTab : public EditorTab
	{
	public:
				AudioEditorTab(std::shared_ptr<Asset> asset);
				~AudioEditorTab() override;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
	};
}
