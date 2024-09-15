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

	/// @brief 
	/// @param pickingId 
	/// @return 
	Color PickingManager::ConvertIdToColor(uint32_t pickingId)
	{
		float r = (pickingId & 0xFF) / 255.0f;
		float g = ((pickingId >> 8) & 0xFF) / 255.0f;
		float b = ((pickingId >> 16) & 0xFF) / 255.0f;

		return Color(r, g, b, 1.0f);
	}

	/// @brief 
	/// @param pickingColor 
	/// @return 
	uint32_t PickingManager::ConvertColorToId(Color pickingColor)
	{
		uint32_t r = static_cast<uint32_t>(std::round(pickingColor.r * 255.0f)) & 0xFF;
		uint32_t g = static_cast<uint32_t>(std::round(pickingColor.g * 255.0f)) & 0xFF;
		uint32_t b = static_cast<uint32_t>(std::round(pickingColor.b * 255.0f)) & 0xFF;

    	return (b << 16) | (g << 8) | r;
	}
}
