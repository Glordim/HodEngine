#pragma once
#include "HodEngine/Renderer/Export.hpp"

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API Semaphore
	{
	public:

						Semaphore() = default;
						Semaphore(const Semaphore&) = delete;
						Semaphore(Semaphore&&) = delete;
		virtual			~Semaphore() = default;

		Semaphore&		operator=(const Semaphore&) = delete;
		Semaphore&		operator=(Semaphore&&) = delete;
	};
}
