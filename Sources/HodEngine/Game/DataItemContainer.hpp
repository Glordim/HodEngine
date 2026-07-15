#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline game
{
	class DataStruct;

	/// @brief Bundles a DataStruct instance with its concrete RTTI type, so both can be
	/// serialized/deserialized together as the "Settings" data block of a DataItem asset/resource.
	class HOD_GAME_API DataItemContainer
	{
		REFLECTED_CLASS_NO_PARENT(DataItemContainer)

	public:

					DataItemContainer() = default;
					DataItemContainer(DataStruct* dataItem);
					DataItemContainer(const DataItemContainer&) = delete;
					DataItemContainer(DataItemContainer&&) = delete;
		virtual		~DataItemContainer();

		DataItemContainer&	operator=(const DataItemContainer&) = delete;
		DataItemContainer&	operator=(DataItemContainer&&) = delete;

		DataStruct*	GetData();

	private:

		DataStruct*	_data = nullptr;
	};
}
