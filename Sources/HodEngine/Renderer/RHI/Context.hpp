#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>
#include <cstdint>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		class HOD_API Context
		{
		public:
							Context();
			virtual			~Context();

		public:

			virtual void	Resize(uint32_t width, uint32_t height) = 0;

			virtual bool	AcquireNextImageIndex() = 0;
			virtual bool	SwapBuffer() = 0;
		};
	}
}
