#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetData.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_GAME_EDITOR_API DataTableEditorTab : public EditorTab
	{
	public:
				DataTableEditorTab(std::shared_ptr<Asset> asset);
				~DataTableEditorTab() override;

		/// @return What it edits. Its rows are valid when IsReadable()
		DataTableAssetData&	GetData();
		/// @return false when its asset couldn't be read (unknown row type): shown, not saved
		bool				IsReadable() const;

		/// @return The row of this key, null without it
		DataStruct*		FindRow(const String& key) const;
		/// @return The new row (default values) at the end, null when the key exists already
		DataStruct*		AddRow(const String& key);
		/// @brief Its selection cleared when it is this row
		void			RemoveRow(const String& key);
		/// @return false (an error) when the new key is empty or taken by another row: the row keeps its key. Its selection follows it
		bool			RenameRow(const String& key, const String& newKey);

		void			SetRowSelection(const String& key);
		const String&	GetRowSelectionKey() const;
		DataStruct*		GetRowSelection() const;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

		/// @brief Undo: its rows (as saved), not the selection (found back by its key)
		bool	CaptureUndoState(Document& document) override;
		bool	RestoreUndoState(const Document& document) override;

	private:

		DataTableAssetData	_data;
		bool				_readable = false;
		String				_rowSelectionKey;
	};
}
