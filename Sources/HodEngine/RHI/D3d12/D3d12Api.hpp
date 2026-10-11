#pragma once

// The only place the D3D12 backend includes the Windows and DirectX headers from

#if !defined(WIN32_LEAN_AND_MEAN)
	#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
	#define NOMINMAX
#endif
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl/client.h>

// Names of the RHI that Windows.h turns into macros
#undef CreateSemaphore
#undef min
#undef max

namespace hod::inline rhi
{
	using Microsoft::WRL::ComPtr;
}
