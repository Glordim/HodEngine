#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/Window/Surface.hpp>

#include <string>
#include <cstdint>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		class HOD_API Context : public window::Surface
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
