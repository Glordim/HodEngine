#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include "HodEngine/Game/DataStruct.hpp"
#include "HodEngine/Game/DataItemContainer.hpp"

namespace hod::inline game
{
	/// @brief
	class HOD_GAME_API DataItemResource : public Resource
	{
		REFLECTED_CLASS(DataItemResource, Resource)

	public:
								DataItemResource() = default;
								DataItemResource(const DataItemResource&) = delete;
								DataItemResource(DataItemResource&&) = delete;
								~DataItemResource() override = default;

		DataItemResource&	operator=(const DataItemResource&) = delete;
		DataItemResource&	operator=(DataItemResource&&) = delete;

	public:

		bool					Initialize(const ResourceContainer& resourceContainer) override;

		DataStruct&			GetDataItem();

	private:

		DataItemContainer	_dataItemContainer;
	};
}
