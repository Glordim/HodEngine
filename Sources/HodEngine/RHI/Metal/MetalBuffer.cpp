#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/RhiDeviceMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	MetalBuffer::MetalBuffer(Usage usage, uint32_t Size)
	: Buffer(usage, Size)
	{
		RhiDeviceMetal* rhiDeviceMetal = RhiDeviceMetal::GetInstance();
		_nativeBuffer = rhiDeviceMetal->GetDevice()->newBuffer(Size, MTL::ResourceStorageModeShared);
		rhiDeviceMetal->AddResourceToResidencySet(_nativeBuffer);
	}

	MetalBuffer::~MetalBuffer()
	{
		RhiDeviceMetal::GetInstance()->RemoveResourceFromResidencySet(_nativeBuffer);
		_nativeBuffer->release();
	}

	bool MetalBuffer::Resize(uint32_t Size)
	{
		RhiDeviceMetal* rhiDeviceMetal = RhiDeviceMetal::GetInstance();
		rhiDeviceMetal->RemoveResourceFromResidencySet(_nativeBuffer);
		_nativeBuffer->release();
		_nativeBuffer = rhiDeviceMetal->GetDevice()->newBuffer(Size, MTL::ResourceStorageModeShared);
		rhiDeviceMetal->AddResourceToResidencySet(_nativeBuffer);
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
