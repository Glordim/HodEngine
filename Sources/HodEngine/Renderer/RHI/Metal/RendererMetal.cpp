#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalContext.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalSemaphore.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalFence.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"
#include "HodEngine/Renderer/RHI/Metal/ShaderGeneratorMetal.hpp"

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

		bool RendererMetal::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore, const Fence* fence)
		{
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
			return new MetalCommandBuffer();
		}

		Buffer* RendererMetal::CreateBuffer(Buffer::Usage usage, uint32_t size)
		{
			return new MetalBuffer(usage, size);
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
			_shaderGenerator = new ShaderGeneratorMetal();

			_device = MTL::CreateSystemDefaultDevice();
			_commandQueue = _device->newCommandQueue();

			_context = new MetalContext(static_cast<window::MacOsWindow*>(mainWindow));
			mainWindow->SetSurface(_context);

/*
			_layer = [CAMetalLayer layer];
			_layer.device = _device->GetNativeDevice();
			_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
			_layer.framebufferOnly = YES;
			_layer.frame = mainWindow->GetSize();

			window::MacOsWindow* macOsWindow = static_cast<window::MacOsWindow*>(mainWindow);
			NSView* view = macOsWindow->GetNSView();

			[view.layer addSublayer:_layer];
*/

			_renderQueue->Init();
			
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererMetal::GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices)
		{
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererMetal::BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier)
		{
			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Shader* RendererMetal::CreateShader(Shader::ShaderType type)
		{
			return new MetalShader(type);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Material* RendererMetal::CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
		{
			MetalMaterial* material = new MetalMaterial();
			if (material->Build(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
			{
				delete material;
				return nullptr;
			}
			return material;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* RendererMetal::CreateMaterialInstance(const Material* material)
		{
			return new MetalMaterialInstance(*material);
		}

		/// @brief 
		/// @return 
		Semaphore* RendererMetal::CreateSemaphore()
		{
			return new MetalSemaphore();
		}

		/// @brief 
		/// @return 
		Fence* RendererMetal::CreateFence()
		{
			return new MetalFence();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Texture* RendererMetal::CreateTexture()
		{
			return new MetalTexture();
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
