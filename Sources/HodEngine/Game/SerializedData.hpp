#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::game
{
	///@brief 
	class HOD_GAME_API SerializedData : public std::enable_shared_from_this<SerializedData>
	{
		REFLECTED_CLASS_NO_PARENT(SerializedData)

	public:

		virtual ~SerializedData() = default;
	};
}
