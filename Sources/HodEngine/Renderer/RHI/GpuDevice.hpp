#pragma once

#include <string>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct GpuDevice
		{
			std::wstring name;
			size_t vram;
			size_t score;
			bool compatible;
		};
	}
}
