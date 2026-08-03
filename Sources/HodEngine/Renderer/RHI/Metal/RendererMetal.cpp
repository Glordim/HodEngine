#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalFence.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline renderer
{
	_SingletonOverrideConstructor(RendererMetal)
	: Renderer()
	{
	}

	/// @brief
	RendererMetal::~RendererMetal()
	{
		_residencySet->release();
		for (MTL4::CommandAllocator* commandAllocator : _commandAllocators)
		{
			commandAllocator->release();
		}
		_commandQueue->release();
		_device->release();
	}

	bool RendererMetal::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore,
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

	CommandBuffer* RendererMetal::CreateCommandBuffer()
	{
		return DefaultAllocator::GetInstance().New<MetalCommandBuffer>();
	}

	Buffer* RendererMetal::CreateBuffer(Buffer::Usage usage, uint32_t Size)
	{
		return DefaultAllocator::GetInstance().New<MetalBuffer>(usage, Size);
	}

	RenderTarget* RendererMetal::CreateRenderTarget()
	{
		return DefaultAllocator::GetInstance().New<RenderTarget>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool RendererMetal::Init(Window* mainWindow, uint32_t physicalDeviceIdentifier)
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

		_mainPresentationSurface = CreatePresentationSurface(mainWindow);

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool RendererMetal::GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices)
	{
		(void)availableDevices; // TODO
		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	PresentationSurface* RendererMetal::CreatePresentationSurface(window::Window* window)
	{
		MetalPresentationSurface* presentationSurface = DefaultAllocator::GetInstance().New<MetalPresentationSurface>(static_cast<MacOsWindow*>(window));
		presentationSurface->Resize(window->GetWidth(), window->GetHeight());
		_presentationSurfaces.PushBack(presentationSurface);
		return presentationSurface;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Shader* RendererMetal::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<MetalShader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Material* RendererMetal::CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
											Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
	{
		MetalMaterial* material = DefaultAllocator::GetInstance().New<MetalMaterial>();
		if (material->Build(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
		{
			DefaultAllocator::GetInstance().Delete(material);
			return nullptr;
		}
		return material;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MaterialInstance* RendererMetal::CreateMaterialInstance(const Material* material)
	{
		return DefaultAllocator::GetInstance().New<MetalMaterialInstance>(*material);
	}

	/// @brief
	/// @return
	Semaphore* RendererMetal::CreateSemaphore()
	{
		return DefaultAllocator::GetInstance().New<MetalSemaphore>();
	}

	/// @brief
	/// @return
	Fence* RendererMetal::CreateFence()
	{
		return DefaultAllocator::GetInstance().New<MetalFence>();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Texture* RendererMetal::CreateTexture()
	{
		return DefaultAllocator::GetInstance().New<MetalTexture>();
	}

	MTL::Device* RendererMetal::GetDevice() const
	{
		return _device;
	}

	MTL4::CommandQueue* RendererMetal::GetCommandQueue() const
	{
		return _commandQueue;
	}

	MTL4::CommandAllocator* RendererMetal::GetCommandAllocator(uint32_t frameIndex) const
	{
		return _commandAllocators[frameIndex];
	}

	void RendererMetal::AddResourceToResidencySet(const MTL::Allocation* allocation)
	{
		_residencySet->addAllocation(allocation);
		_residencySet->commit();
	}

	void RendererMetal::FlushDeferredDeletions(uint32_t frameIndex)
	{
		_commandAllocators[frameIndex]->reset();
	}
}
