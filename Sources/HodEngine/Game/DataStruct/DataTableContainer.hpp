#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Rtti.hpp>
#include <HodEngine/Core/String.hpp>

#include <utility>
#include <vector>

namespace hod::inline game
{
	class DataStruct;

	/// @brief Bundles a table of DataStruct instances, all of the same concrete RTTI type (the row type,
	/// stored alongside the table so it is known even when the table has no row yet), indexed by a string
	/// key, so the whole table can be serialized/deserialized together as the "Settings" data block of a
	/// DataTable asset/resource.
	class HOD_GAME_API DataTableContainer
	{
		REFLECTED_CLASS_NO_PARENT(DataTableContainer)

	public:

					DataTableContainer() = default;
					DataTableContainer(RttiType rowType);
					DataTableContainer(const DataTableContainer&) = delete;
					DataTableContainer(DataTableContainer&&) = delete;
		virtual		~DataTableContainer();

		DataTableContainer&	operator=(const DataTableContainer&) = delete;
		DataTableContainer&	operator=(DataTableContainer&&) = delete;

		RttiType	GetRowType() const;
		void		SetRowType(RttiType rowType);

		DataStruct*	AddRow(const String& key);
		void		RemoveRow(const String& key);
		DataStruct*	FindRow(const String& key);

		std::vector<std::pair<String, DataStruct*>>&		GetRows();
		const std::vector<std::pair<String, DataStruct*>>&	GetRows() const;

	private:

		RttiType										_rowType = 0;
		std::vector<std::pair<String, DataStruct*>>	_rows;
	};
}
