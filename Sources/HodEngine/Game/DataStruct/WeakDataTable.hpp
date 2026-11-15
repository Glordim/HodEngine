#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/DataStruct/DataTableResource.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>

#include <memory>
#include <string_view>
#include <type_traits>

namespace hod::inline game
{
	/// @brief A data table asset whose rows are _DataStruct_: the editor's pickers only offer those, FindRow reads a row typed.
	/// Serialized as any WeakResource (its UID)
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

		/// @return Its table. Null without an asset, or when its rows are of another struct (an error)
		std::shared_ptr<DataTableResource> Lock() const
		{
			std::shared_ptr<DataTableResource> resource = WeakResource<DataTableResource>::Lock();
			if (resource == nullptr)
			{
				return nullptr;
			}

			if (resource->GetRowType() != _DataStruct_::GetReflectionDescriptor().GetType())
			{
				OUTPUT_ERROR("Data table {}: its rows aren't {}", GetUid().ToString(), _DataStruct_::GetReflectionDescriptor().GetTypeName());
				return nullptr;
			}
			return resource;
		}

		/// @return The row of the key, null without it (or without a table). Alive while the table is referenced
		const _DataStruct_* FindRow(std::string_view key) const
		{
			std::shared_ptr<DataTableResource> resource = Lock();
			return (resource != nullptr) ? resource->FindRow<_DataStruct_>(key) : nullptr;
		}

		/// @return The row of the key's hash (Hash::ComputeXxh3_64), null without it (or without a table)
		const _DataStruct_* FindRow(uint64_t keyHash) const
		{
			std::shared_ptr<DataTableResource> resource = Lock();
			return (resource != nullptr) ? resource->FindRow<_DataStruct_>(keyHash) : nullptr;
		}
	};

	template<typename _DataStruct_>
	void WeakDataTable<_DataStruct_>::FillReflectionDescriptorUser([[maybe_unused]] ReflectionDescriptor& reflectionDescriptor)
	{
	}
}
