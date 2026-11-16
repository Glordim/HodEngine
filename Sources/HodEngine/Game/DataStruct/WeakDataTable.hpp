#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/DataStruct/DataTableResource.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>

#include <memory>
#include <string_view>
#include <type_traits>

namespace hod::inline game
{
	/// @brief A data table asset whose rows are _DataStruct_, read as an array of them: `for (const T& row : _table)`,
	/// `_table[index]`, `_table.FindRow("Key")`. The editor's pickers only offer those. Serialized as any WeakResource (its UID).
	/// Without a table, or when its rows are of another struct (an error, once per asset), it reads as an empty table.
	/// A row is valid while the WeakDataTable references its table (it keeps it alive once resolved)
	template<typename _DataStruct_>
	class WeakDataTable : public WeakResource<DataTableResource>
	{
		REFLECTED_CLASS(WeakDataTable<_DataStruct_>, WeakResource<DataTableResource>)

		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "WeakDataTable<T>: T is a DataStruct");

	public:
		RttiType GetExpectedSubType() const override
		{
			return _DataStruct_::GetReflectionDescriptor().GetType();
		}

		uint32_t GetRowCount() const
		{
			const DataTableResource* table = GetTable();
			return (table != nullptr) ? table->GetRowCount() : 0;
		}

		/// @return The row at this index, in the table's order (asserted in range)
		const _DataStruct_& operator[](uint32_t index) const
		{
			const DataTableResource* table = GetTable();
			Assert(table != nullptr && index < table->GetRowCount());
			return table->GetRows<_DataStruct_>()[index];
		}

		/// @return The row of the key, null without it
		const _DataStruct_* FindRow(std::string_view key) const
		{
			const DataTableResource* table = GetTable();
			return (table != nullptr) ? table->FindRow<_DataStruct_>(key) : nullptr;
		}

		/// @return The row of the key's hash (Hash::ComputeXxh3_64), null without it
		const _DataStruct_* FindRow(uint64_t keyHash) const
		{
			const DataTableResource* table = GetTable();
			return (table != nullptr) ? table->FindRow<_DataStruct_>(keyHash) : nullptr;
		}

		/// @brief Its rows in the table's order
		const _DataStruct_* begin() const
		{
			const DataTableResource* table = GetTable();
			return (table != nullptr) ? table->GetRows<_DataStruct_>() : nullptr;
		}

		const _DataStruct_* end() const
		{
			const DataTableResource* table = GetTable();
			return (table != nullptr) ? table->GetRows<_DataStruct_>() + table->GetRowCount() : nullptr;
		}

	private:
		/// @return Its table (alive while referenced: Lock keeps it), null without one or when its rows aren't _DataStruct_
		const DataTableResource* GetTable() const
		{
			std::shared_ptr<DataTableResource> resource = Lock();
			if (resource == nullptr)
			{
				return nullptr;
			}

			// Checked once per table resolved, not on each access
			if (resource.get() != _checkedTable || resource->GetUid() != _checkedUid)
			{
				_checkedTable = resource.get();
				_checkedUid = resource->GetUid();
				_rowTypeMatches = (resource->GetRowType() == _DataStruct_::GetReflectionDescriptor().GetType());
				if (_rowTypeMatches == false)
				{
					OUTPUT_ERROR("Data table {}: its rows aren't {}", GetUid().ToString(), _DataStruct_::GetReflectionDescriptor().GetTypeName());
				}
			}
			return _rowTypeMatches ? resource.get() : nullptr;
		}

	private:
		mutable const DataTableResource* _checkedTable = nullptr;
		mutable UID                      _checkedUid;
		mutable bool                     _rowTypeMatches = false;
	};

	template<typename _DataStruct_>
	void WeakDataTable<_DataStruct_>::FillReflectionDescriptorUser([[maybe_unused]] ReflectionDescriptor& reflectionDescriptor)
	{
	}
}
