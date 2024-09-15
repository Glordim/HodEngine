#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <string>
#include <unordered_map>

#include <HodEngine/Core/Singleton.hpp>

#include <utility>
#include <atomic>

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API PickingManager
	{
		_Singleton(PickingManager)

	public:

		static constexpr uint32_t InvalidId = 0;

	public:

		uint32_t GenerateId();

	private:

		std::atomic<uint32_t>	_nextId = { 1 };
	};
}
