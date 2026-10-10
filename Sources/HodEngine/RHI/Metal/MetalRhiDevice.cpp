#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Metal/MetalFence.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"
#include "HodEngine/RHI/Metal/MetalGraphicsPipeline.hpp"
#include "HodEngine/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/RHI/Metal/MetalShader.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	_SingletonOverrideConstructor(MetalRhiDevice)
	: RhiDevice()
	{
	}

	/// @brief
	MetalRhiDevice::~MetalRhiDevice()
	{
		_residencySet->release();
		for (MTL4::CommandAllocator* commandAllocator : _commandAllocators)
		{
			commandAllocator->release();
		}
		_commandQueue->release();
		_device->release();
	}

	bool MetalRhiDevice::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore,
												const Fence* fence)
	{
		MTL4::CommandBuffer** mtlCommandBuffers = (MTL4::CommandBuffer**)alloca(sizeof(MTL4::CommandBuffer*) * commandBufferCount);
		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
		{
			MetalCommandBuffer* commandBuffer = static_cast<MetalCommandBuffer*>(commandBuffers[commandBufferIndex]);
			mtlCommandBuffers[commandBufferIndex] = commandBuffer->GetNativeCommandBuffer();
		}

		if (waitSemaphore != nullptr)
		{
			const MetalSemaphore* metalSemaphore = static_cast<const MetalSemaphore*>(waitSemaphore);
			_commandQueue->wait(metalSemaphore->GetNativeSemaphore(), metalSemaphore->GetTargetValue());
		}

		_commandQueue->commit(mtlCommandBuffers, commandBufferCount);

		if (signalSemaphore != nullptr)
		{
			MetalSemaphore* metalSemaphore = static_cast<MetalSemaphore*>(const_cast<Semaphore*>(signalSemaphore));
			_commandQueue->signalEvent(metalSemaphore->GetNativeSemaphore(), metalSemaphore->IncrementAndGetTargetValue());
		}

		if (fence != nullptr)
		{
			const MetalFence* metalFence = static_cast<const MetalFence*>(fence);
			_commandQueue->signalEvent(metalFence->GetNativeEvent(), metalFence->GetTargetValue());
		}

		return true;
	}

	CommandBuffer* MetalRhiDevice::CreateCommandBuffer()
	{
		return DefaultAllocator::GetInstance().New<MetalCommandBuffer>();
	}

	Buffer* MetalRhiDevice::CreateBuffer(Buffer::Usage usage, uint32_t Size)
	{
		return DefaultAllocator::GetInstance().New<MetalBuffer>(usage, Size);
	}

	RenderTarget* MetalRhiDevice::CreateRenderTarget()
	{
		return DefaultAllocator::GetInstance().New<RenderTarget>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalRhiDevice::InitDevice(uint32_t physicalDeviceIdentifier)
	{
		(void)physicalDeviceIdentifier; // TODO

		_device = MTL::CreateSystemDefaultDevice();

		if (_device->supportsFamily(MTL::GPUFamilyMetal4) == false)
		{
			OUTPUT_ERROR("Metal: This GPU does not support Metal 4");
			return false;
		}

		_commandQueue = _device->newMTL4CommandQueue();

		_commandAllocators.Resize(GetFrameInFlightCount());
		for (MTL4::CommandAllocator*& commandAllocator : _commandAllocators)
		{
			commandAllocator = _device->newCommandAllocator();
		}

		MTL::ResidencySetDescriptor* residencySetDescriptor = MTL::ResidencySetDescriptor::alloc()->init();
		NS::Error*                   residencySetError = nullptr;
		_residencySet = _device->newResidencySet(residencySetDescriptor, &residencySetError);
		residencySetDescriptor->release();
		if (_residencySet == nullptr)
		{
			OUTPUT_ERROR("Metal: Unable to create residency set: {}", residencySetError->localizedDescription()->utf8String());
			return false;
		}
		_commandQueue->addResidencySet(_residencySet);

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool MetalRhiDevice::GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices)
	{
		(void)availableDevices; // TODO
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	PresentationSurface* MetalRhiDevice::CreatePresentationSurface(window::Window* window)
	{
		MetalPresentationSurface* presentationSurface = DefaultAllocator::GetInstance().New<MetalPresentationSurface>(static_cast<MacOsWindow*>(window));
		presentationSurface->Resize(window->GetWidth(), window->GetHeight());
		_presentationSurfaces.PushBack(presentationSurface);
		return presentationSurface;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Shader* MetalRhiDevice::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<MetalShader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	GraphicsPipeline* MetalRhiDevice::CreateGraphicsPipeline(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
											GraphicsPipeline::PolygonMode polygonMode, GraphicsPipeline::Topololy topololy, bool useDepth)
	{
		MetalGraphicsPipeline* graphicsPipeline = DefaultAllocator::GetInstance().New<MetalGraphicsPipeline>();
		if (graphicsPipeline->Build(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
		{
			DefaultAllocator::GetInstance().Delete(graphicsPipeline);
			return nullptr;
		}
		return graphicsPipeline;
	}

	/// @brief
	/// @return
	Semaphore* MetalRhiDevice::CreateSemaphore()
	{
		return DefaultAllocator::GetInstance().New<MetalSemaphore>();
	}

	/// @brief
	/// @return
	Fence* MetalRhiDevice::CreateFence()
	{
		return DefaultAllocator::GetInstance().New<MetalFence>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Texture* MetalRhiDevice::CreateTexture()
	{
		return DefaultAllocator::GetInstance().New<MetalTexture>();
	}

	MTL::Device* MetalRhiDevice::GetDevice() const
	{
		return _device;
	}

	MTL4::CommandQueue* MetalRhiDevice::GetCommandQueue() const
	{
		return _commandQueue;
	}

	MTL4::CommandAllocator* MetalRhiDevice::GetCommandAllocator(uint32_t frameIndex) const
	{
		return _commandAllocators[frameIndex];
	}

	void MetalRhiDevice::AddResourceToResidencySet(const MTL::Allocation* allocation)
	{
		_residencySet->addAllocation(allocation);
		_residencySet->commit();
	}

	void MetalRhiDevice::RemoveResourceFromResidencySet(const MTL::Allocation* allocation)
	{
		_residencySet->removeAllocation(allocation);
		_residencySet->commit();
	}

	/// @brief
	BindGroup* MetalRhiDevice::CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
	                                           const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount)
	{
		// TODO
		(void)graphicsPipeline;
		(void)set;
		(void)uniformBuffers;
		(void)uniformBufferCount;
		(void)textureBindings;
		(void)textureBindingCount;
		return nullptr;
	}

	/// @brief
	Sampler* MetalRhiDevice::CreateSampler(const Sampler::CreateInfo& createInfo)
	{
		// TODO
		(void)createInfo;
		return nullptr;
	}

	/// @brief
	uint32_t MetalRhiDevice::GetUniformBufferOffsetAlignment() const
	{
		return 256; // TODO
	}

	void MetalRhiDevice::FlushDeferredDeletions(uint32_t frameIndex)
	{
		_commandAllocators[frameIndex]->reset();
	}
}
