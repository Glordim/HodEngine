#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct HOD_API GpuDevice
		{
			std::wstring name;
			size_t vram;
			size_t score;
			bool compatible;
		};
	}
}
