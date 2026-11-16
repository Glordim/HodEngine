#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include "HodEngine/Game/DataStruct/DataStruct.hpp"

namespace hod::inline game
{
	/// @brief A data item: one DataStruct, its type written with it (Serializer::SerializeWithType, the asset's "Settings" block as is)
	class HOD_GAME_API DataItemResource : public Resource
	{
		REFLECTED_CLASS(DataItemResource, Resource)

	public:
								DataItemResource() = default;
								DataItemResource(const DataItemResource&) = delete;
								DataItemResource(DataItemResource&&) = delete;
								~DataItemResource() override;

		DataItemResource&	operator=(const DataItemResource&) = delete;
		DataItemResource&	operator=(DataItemResource&&) = delete;

	public:

		bool					Initialize(const ResourceContainer& resourceContainer) override;

		/// @return Its data
		const DataStruct*	GetDataItem() const;

	private:

		DataStruct*			_data = nullptr;
	};
}
