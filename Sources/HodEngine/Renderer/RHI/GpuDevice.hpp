#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <string>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct HOD_RENDERER_API GpuDevice
		{
			std::wstring name;
			size_t vram;
			size_t score;
			bool compatible;
		};
	}
}
