#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	MetalBuffer::MetalBuffer(Usage usage, uint32_t Size)
	: Buffer(usage, Size)
	{
		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();
		_nativeBuffer = metalRhiDevice->GetDevice()->newBuffer(Size, MTL::ResourceStorageModeShared);
		metalRhiDevice->AddResourceToResidencySet(_nativeBuffer);
	}

	MetalBuffer::~MetalBuffer()
	{
		MetalRhiDevice::GetInstance()->RemoveResourceFromResidencySet(_nativeBuffer);
		_nativeBuffer->release();
	}

	bool MetalBuffer::Resize(uint32_t Size)
	{
		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();
		metalRhiDevice->RemoveResourceFromResidencySet(_nativeBuffer);
		_nativeBuffer->release();
		_nativeBuffer = metalRhiDevice->GetDevice()->newBuffer(Size, MTL::ResourceStorageModeShared);
		metalRhiDevice->AddResourceToResidencySet(_nativeBuffer);
		_size = Size;
		return true;
	}

	void* MetalBuffer::Lock()
	{
		return _nativeBuffer->contents();
	}

	void MetalBuffer::Unlock() {}

	MTL::Buffer* MetalBuffer::GetNativeBuffer() const
	{
		return _nativeBuffer;
	}
}
