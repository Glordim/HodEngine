#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_RHI_API GpuDevice
	{
		std::wstring name;
		size_t vram;
		size_t score;
		bool compatible;
	};
}
