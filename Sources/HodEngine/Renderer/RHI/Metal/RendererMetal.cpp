#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalFence.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"

#include "HodEngine/Window/Desktop/MacOs/MacOsWindow.hpp"

#include <Metal/Metal.hpp>

namespace hod
{
	namespace renderer
	{
		_SingletonOverrideConstructor(RendererMetal)
		: Renderer()
		{
		}

		/// @brief
		RendererMetal::~RendererMetal()
		{
			_commandQueue->release();
			_device->release();
		}

		bool RendererMetal::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore,
		                                         const Fence* fence)
		{
			// TODO
			(void)signalSemaphore;
			(void)waitSemaphore;
			(void)fence;

			for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBufferCount; ++commandBufferIndex)
			{
				MetalCommandBuffer* commandBuffer = static_cast<MetalCommandBuffer*>(commandBuffers[commandBufferIndex]);
				MTL::CommandBuffer* metalCommandBuffer = commandBuffer->GetNativeCommandBuffer();
				metalCommandBuffer->commit();
				metalCommandBuffer->waitUntilCompleted();
			}
			return false;
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
			return nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		bool RendererMetal::Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier)
		{
			(void)physicalDeviceIdentifier; // TODO

			_device = MTL::CreateSystemDefaultDevice();
			_commandQueue = _device->newCommandQueue();

			MetalPresentationSurface* presentationSurface = DefaultAllocator::GetInstance().New<MetalPresentationSurface>(static_cast<window::MacOsWindow*>(mainWindow));
			presentationSurface->Resize(mainWindow->GetWidth(), mainWindow->GetHeight());
			_presentationSurfaces.PushBack(presentationSurface);

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

		MTL::CommandQueue* RendererMetal::GetCommandQueue() const
		{
			return _commandQueue;
		}
	}
}
