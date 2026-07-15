#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Game/DataItemContainer.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API DataItemEditorTab : public EditorTab
	{
	public:
				DataItemEditorTab(std::shared_ptr<Asset> asset);
				~DataItemEditorTab() override;

		DataItemContainer&	GetDataItemContainer();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		DataItemContainer	_dataItemContainer;
	};
}
