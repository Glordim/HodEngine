#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::inline game
{
	class SerializedData;

	/// @brief Bundles a SerializedData instance with its concrete RTTI type, so both can be
	/// serialized/deserialized together as the "Settings" data block of a SerializedData asset/resource.
	class HOD_GAME_API SerializedDataContainer
	{
		REFLECTED_CLASS_NO_PARENT(SerializedDataContainer)

	public:

					SerializedDataContainer() = default;
					SerializedDataContainer(SerializedData* serializedData);
					SerializedDataContainer(const SerializedDataContainer&) = delete;
					SerializedDataContainer(SerializedDataContainer&&) = delete;
		virtual		~SerializedDataContainer();

		SerializedDataContainer&	operator=(const SerializedDataContainer&) = delete;
		SerializedDataContainer&	operator=(SerializedDataContainer&&) = delete;

		SerializedData*	GetData();

	private:

		SerializedData*	_data = nullptr;
	};
}
