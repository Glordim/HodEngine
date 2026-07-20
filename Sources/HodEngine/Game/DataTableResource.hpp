#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/Core/Reflection/Rtti.hpp>
#include <HodEngine/Core/String.hpp>
#include "HodEngine/Game/DataStruct.hpp"

#include <unordered_map>
#include <string_view>
#include <type_traits>

namespace hod::inline game
{
	/// @brief Runtime-friendly counterpart of DataTableContainer: rows are indexed by the xxh3 hash of
	/// their key instead of a linear search, since lookups happen every frame at runtime while the
	/// editor-side container favors preserving row order for display/edition.
	class HOD_GAME_API DataTableResource : public Resource
	{
	public:
								DataTableResource() = default;
								DataTableResource(const DataTableResource&) = delete;
								DataTableResource(DataTableResource&&) = delete;
								~DataTableResource() override;

		DataTableResource&	operator=(const DataTableResource&) = delete;
		DataTableResource&	operator=(DataTableResource&&) = delete;

	public:

		bool			Initialize(const ResourceContainer& resourceContainer) override;

		RttiType		GetRowType() const;

		DataStruct*		FindRow(uint64_t keyHash) const;
		DataStruct*		FindRow(std::string_view key) const;

		template<typename _DataStruct_>
		_DataStruct_*	FindRow(uint64_t keyHash) const;
		template<typename _DataStruct_>
		_DataStruct_*	FindRow(std::string_view key) const;

		const std::unordered_map<uint64_t, DataStruct*>&	GetRows() const;

	private:

		RttiType									_rowType = 0;
		std::unordered_map<uint64_t, DataStruct*>	_rows;
	};

	template<typename _DataStruct_>
	_DataStruct_* DataTableResource::FindRow(uint64_t keyHash) const
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use FindRow()");
		return static_cast<_DataStruct_*>(FindRow(keyHash));
	}

	template<typename _DataStruct_>
	_DataStruct_* DataTableResource::FindRow(std::string_view key) const
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use FindRow()");
		return static_cast<_DataStruct_*>(FindRow(key));
	}
}
