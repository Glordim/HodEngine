#pragma once
#include "HodEngine/Renderer/Export.hpp"

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API Fence
	{
	public:

						Fence() = default;
						Fence(const Fence&) = delete;
						Fence(Fence&&) = delete;
		virtual			~Fence() = default;

		Fence&			operator=(const Fence&) = delete;
		Fence&			operator=(Fence&&) = delete;

	public:

		virtual bool	Reset() = 0;
		virtual bool	Wait() = 0;
	};
}
