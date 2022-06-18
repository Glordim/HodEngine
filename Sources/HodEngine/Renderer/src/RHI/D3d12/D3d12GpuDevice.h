#pragma once

#if defined(_WIN32)

#include "../GpuDevice.h"

#include <dxgi1_5.h>

#include <wrl.h>
using namespace Microsoft::WRL;

namespace hod
{
	namespace renderer
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

#endif
