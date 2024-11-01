#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Resource.hpp>
#include "HodEngine/Game/SerializedData.hpp"

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API SerializedDataResource : public Resource
	{
		REFLECTED_CLASS(SerializedDataResource, Resource, HOD_GAME_API)

	public:
								SerializedDataResource() = default;
								SerializedDataResource(const SerializedDataResource&) = delete;
								SerializedDataResource(SerializedDataResource&&) = delete;
								~SerializedDataResource() override;

		SerializedDataResource&	operator=(const SerializedDataResource&) = delete;
		SerializedDataResource&	operator=(SerializedDataResource&&) = delete;

	public:

		bool					Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		SerializedData&			GetSerializedData();

	private:

		SerializedData*			_serializedData = nullptr;
	};
}
