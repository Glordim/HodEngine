#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Game/DataStruct/DataTableContainer.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API DataTableEditorTab : public EditorTab
	{
	public:
				DataTableEditorTab(std::shared_ptr<Asset> asset);
				~DataTableEditorTab() override;

		DataTableContainer&	GetDataTableContainer();

		void			SetRowSelection(const String& key);
		const String&	GetRowSelectionKey() const;
		DataStruct*		GetRowSelection() const;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		DataTableContainer	_dataTableContainer;
		String				_rowSelectionKey;
	};
}
