#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Window/Surface.hpp>

#include <string>
#include <cstdint>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		class HOD_RENDERER_API Context : public window::Surface
		{
		public:
							Context();
			virtual			~Context();

		public:

			virtual bool	AcquireNextImageIndex() = 0;
			virtual bool	SwapBuffer() = 0;
		};
	}
}
