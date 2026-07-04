#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include "HodEngine/Game/SerializedData.hpp"

namespace hod::inline game
{
	/// @brief 
	class HOD_GAME_API SerializedDataResource : public Resource
	{
		REFLECTED_CLASS(SerializedDataResource, Resource)

	public:
								SerializedDataResource() = default;
								SerializedDataResource(const SerializedDataResource&) = delete;
								SerializedDataResource(SerializedDataResource&&) = delete;
								~SerializedDataResource() override;

		SerializedDataResource&	operator=(const SerializedDataResource&) = delete;
		SerializedDataResource&	operator=(SerializedDataResource&&) = delete;

	public:

		bool					Initialize(const ResourceContainer& resourceContainer) override;

		SerializedData&			GetSerializedData();

	private:

		SerializedData*			_serializedData = nullptr;
	};
}
