#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include <memory>

namespace hod::inline game
{
	///@brief
	class HOD_GAME_API DataStruct : public std::enable_shared_from_this<DataStruct>
	{
		REFLECTED_CLASS_NO_PARENT(DataStruct)

	public:

		virtual ~DataStruct() = default;
	};
}
