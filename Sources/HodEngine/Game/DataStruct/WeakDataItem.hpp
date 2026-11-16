#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/DataStruct/DataItemResource.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>

#include <memory>
#include <type_traits>

namespace hod::inline game
{
	/// @brief A data item asset holding a _DataStruct_ (or a struct deriving from it), read typed by Get(). The editor's pickers
	/// only offer those. Serialized as any WeakResource (its UID).
	/// Its data is valid while the WeakDataItem references its asset (it keeps it alive once resolved)
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

		/// @return Its data. Null without an asset, or when the asset holds another struct (an error, once per asset)
		const _DataStruct_* Get() const
		{
			std::shared_ptr<DataItemResource> resource = Lock();
			if (resource == nullptr)
			{
				return nullptr;
			}

			const DataStruct* data = resource->GetDataItem();

			// Checked once per asset resolved, not on each access
			if (resource.get() != _checkedItem || resource->GetUid() != _checkedUid)
			{
				_checkedItem = resource.get();
				_checkedUid = resource->GetUid();
				_dataMatches = (data != nullptr && data->GetReflectionDescriptorV().IsCompatible(_DataStruct_::GetReflectionDescriptor()));
				if (_dataMatches == false)
				{
					OUTPUT_ERROR("Data item {}: holds {}, not {}", GetUid().ToString(), (data != nullptr) ? data->GetReflectionDescriptorV().GetTypeName() : String("nothing"),
					             _DataStruct_::GetReflectionDescriptor().GetTypeName());
				}
			}
			return _dataMatches ? static_cast<const _DataStruct_*>(data) : nullptr;
		}

	private:
		mutable const DataItemResource* _checkedItem = nullptr;
		mutable UID                     _checkedUid;
		mutable bool                    _dataMatches = false;
	};

	template<typename _DataStruct_>
	void WeakDataItem<_DataStruct_>::FillReflectionDescriptorUser([[maybe_unused]] ReflectionDescriptor& reflectionDescriptor)
	{
	}
}
