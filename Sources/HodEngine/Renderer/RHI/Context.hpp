#pragma once

#include <string>

namespace hod
{
	namespace renderer
	{
		/// @brief 
		class Context
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
