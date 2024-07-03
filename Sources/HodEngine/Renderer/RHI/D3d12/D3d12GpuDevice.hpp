#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(_WIN32)

#include "HodEngine/Renderer/RHI/GpuDevice.hpp"

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
		struct HOD_API D3d12GpuDevice : public GpuDevice
		{
			ComPtr<IDXGIAdapter1> adapter;
		};
	}
}

#endif
