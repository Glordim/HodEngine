#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/Core/Reflection/Rtti.hpp>
#include <HodEngine/Core/Vector.hpp>
#include "HodEngine/Game/DataStruct/DataStruct.hpp"

#include <cstdint>
#include <new>
#include <string_view>
#include <type_traits>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline game
{
	/// @brief A data table at runtime (cooked from the editor's DataTableAssetData), read only: its rows constructed in one block,
	/// in the table's order (GetRow(index), GetRows<T>() as an array), and its key hashes sorted by the cooker (FindRow, a binary
	/// search), the keys themselves aren't cooked. Read typed through DataTableRef<T>
	class HOD_GAME_API DataTableResource : public Resource
	{
		REFLECTED_CLASS(DataTableResource, Resource)

	public:
								DataTableResource() = default;
								DataTableResource(const DataTableResource&) = delete;
								DataTableResource(DataTableResource&&) = delete;
								~DataTableResource() override;

		DataTableResource&	operator=(const DataTableResource&) = delete;
		DataTableResource&	operator=(DataTableResource&&) = delete;

	public:

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		RttiType			GetRowType() const;

		uint32_t			GetRowCount() const;
		/// @return The row at this index, in the table's order (asserted in range)
		const DataStruct*	GetRow(uint32_t index) const;

		/// @return Its rows as an array, in the table's order (GetRowCount() of them). Its rows are exactly _DataStruct_ (asserted)
		template<typename _DataStruct_>
		const _DataStruct_*	GetRows() const;

		/// @return The row of the key's hash (Hash::ComputeXxh3_64), null without it
		const DataStruct*	FindRow(uint64_t keyHash) const;
		const DataStruct*	FindRow(std::string_view key) const;

		template<typename _DataStruct_>
		const _DataStruct_*	FindRow(uint64_t keyHash) const;
		template<typename _DataStruct_>
		const _DataStruct_*	FindRow(std::string_view key) const;

	private:

		bool				CheckRowType(RttiType type) const;

	private:

		RttiType					_rowType = 0;
		const ReflectionDescriptor*	_rowDescriptor = nullptr;

		uint8_t*					_rows = nullptr; // _rowCount rows of _rowStride bytes
		uint32_t					_rowCount = 0;
		uint32_t					_rowStride = 0;

		Vector<uint64_t>			_sortedKeyHashes;
		Vector<uint32_t>			_rowIndexOfSortedKeys; // The row of each of _sortedKeyHashes
	};

	template<typename _DataStruct_>
	const _DataStruct_* DataTableResource::GetRows() const
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use GetRows()");
		if (CheckRowType(_DataStruct_::GetReflectionDescriptor().GetType()) == false)
		{
			return nullptr;
		}
		return std::launder(reinterpret_cast<const _DataStruct_*>(_rows));
	}

	template<typename _DataStruct_>
	const _DataStruct_* DataTableResource::FindRow(uint64_t keyHash) const
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use FindRow()");
		return static_cast<const _DataStruct_*>(FindRow(keyHash));
	}

	template<typename _DataStruct_>
	const _DataStruct_* DataTableResource::FindRow(std::string_view key) const
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use FindRow()");
		return static_cast<const _DataStruct_*>(FindRow(key));
	}
}
