#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Game/DataStruct/DataStruct.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_GAME_EDITOR_API DataItemEditorTab : public EditorTab
	{
	public:
				DataItemEditorTab(std::shared_ptr<Asset> asset);
				~DataItemEditorTab() override;

		/// @return Its data, null when its struct is unknown (its module not loaded)
		DataStruct*	GetDataItem();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		DataStruct*	_data = nullptr;
	};
}
