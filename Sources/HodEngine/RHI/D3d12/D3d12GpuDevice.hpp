#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/GpuDevice.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	struct HOD_RHI_API D3d12GpuDevice : public GpuDevice
	{
		ComPtr<IDXGIAdapter1> adapter;
		uint32_t              deviceId = 0;
	};
}
