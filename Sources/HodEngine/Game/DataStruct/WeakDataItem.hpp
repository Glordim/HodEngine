#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/DataStruct/DataItemResource.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>

#include <memory>
#include <type_traits>

namespace hod::inline game
{
	/// @brief A data item asset holding a _DataStruct_ (or a struct deriving from it): the editor's pickers only offer those,
	/// Lock() reads it typed. Serialized as any WeakResource (its UID)
	template<typename _DataStruct_>
	class WeakDataItem : public WeakResource<DataItemResource>
	{
		REFLECTED_CLASS(WeakDataItem<_DataStruct_>, WeakResource<DataItemResource>)

		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "WeakDataItem<T>: T is a DataStruct");

	public:
		RttiType GetExpectedSubType() const override
		{
			return _DataStruct_::GetReflectionDescriptor().GetType();
		}

		/// @return Its data, keeping its resource alive. Null without an asset, or when the asset holds another struct (an error)
		std::shared_ptr<const _DataStruct_> Lock() const
		{
			std::shared_ptr<DataItemResource> resource = WeakResource<DataItemResource>::Lock();
			if (resource == nullptr)
			{
				return nullptr;
			}

			const DataStruct* data = resource->GetDataItem();
			if (data == nullptr || data->GetReflectionDescriptorV().IsCompatible(_DataStruct_::GetReflectionDescriptor()) == false)
			{
				OUTPUT_ERROR("Data item {}: holds {}, not {}", GetUid().ToString(), (data != nullptr) ? data->GetReflectionDescriptorV().GetTypeName() : String("nothing"),
				             _DataStruct_::GetReflectionDescriptor().GetTypeName());
				return nullptr;
			}
			return std::shared_ptr<const _DataStruct_>(resource, static_cast<const _DataStruct_*>(data));
		}
	};

	template<typename _DataStruct_>
	void WeakDataItem<_DataStruct_>::FillReflectionDescriptorUser([[maybe_unused]] ReflectionDescriptor& reflectionDescriptor)
	{
	}
}
