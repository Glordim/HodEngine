#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/PickingManager.hpp"

namespace hod::renderer
{
	_SingletonConstructor(PickingManager)
	{

	}

	/// @brief 
	/// @return 
	uint32_t PickingManager::GenerateId()
	{
		return _nextId.fetch_add(1);
	}
}
