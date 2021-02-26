#pragma once

#include "../GpuDevice.h"

#include <dxgi1_5.h>

#include <wrl.h>
using namespace Microsoft::WRL;

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct D3d12GpuDevice : public GpuDevice
		{
			ComPtr<IDXGIAdapter1> adapter;
		};
	}
}
