#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <HodEngine/Core/Reflection/Rtti.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline game
{
	class DataStruct;
}

namespace hod::inline editor
{
	/// @brief What a data table asset holds (its "Settings" block): its row type and its rows, keyed, in the table's order.
	/// No rule of its own: the data table tab edits it (unique keys...), the cooker reads it. Owns its rows
	struct HOD_GAME_EDITOR_API DataTableAssetData
	{
		struct Row
		{
			String      _key;
			DataStruct* _data = nullptr;
		};

		DataTableAssetData() = default;
		DataTableAssetData(const DataTableAssetData&) = delete;
		DataTableAssetData(DataTableAssetData&&) = delete;
		~DataTableAssetData();

		DataTableAssetData& operator=(const DataTableAssetData&) = delete;
		DataTableAssetData& operator=(DataTableAssetData&&) = delete;

		/// @brief Deletes its rows
		void Clear();

		/// @brief Its format: {"Type": row RttiType, "Rows": {"<key>": {row properties}, ...}}
		/// @return false (an error) when its row type is unknown (its module not loaded: _rowType kept, no rows) or a row can't be read
		bool Read(const DocumentNode& documentNode);
		bool Write(DocumentNode& documentNode) const;

		RttiType    _rowType = 0;
		Vector<Row> _rows;
	};
}
