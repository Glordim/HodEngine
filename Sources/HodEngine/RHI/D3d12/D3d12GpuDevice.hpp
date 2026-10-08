#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/GpuDevice.hpp"

#include <dxgi1_5.h>

#include <wrl.h>
using namespace Microsoft::WRL;

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	struct HOD_RHI_API D3d12GpuDevice : public GpuDevice
	{
		ComPtr<IDXGIAdapter1> adapter;
	};
}
