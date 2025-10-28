#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <Metal/Metal.hpp>

namespace hod
{
	namespace renderer
	{
		MetalBuffer::MetalBuffer(Usage usage, uint32_t Size)
		: Buffer(usage, Size)
		{
			RendererMetal* rendererMetal = RendererMetal::GetInstance();
			_nativeBuffer = rendererMetal->GetDevice()->newBuffer(Size, MTL::ResourceStorageModeShared);
		}

		MetalBuffer::~MetalBuffer()
		{
			_nativeBuffer->release();
		}

		bool MetalBuffer::Resize(uint32_t Size)
		{
			_nativeBuffer->release();
			RendererMetal* rendererMetal = RendererMetal::GetInstance();
			_nativeBuffer = rendererMetal->GetDevice()->newBuffer(Size, MTL::ResourceStorageModeShared);
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
}
