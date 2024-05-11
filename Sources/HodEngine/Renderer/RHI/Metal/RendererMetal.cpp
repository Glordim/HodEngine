#include "HodEngine/Renderer/RHI/Metal/RendererMetal.hpp"

#if defined(PLATFORM_MACOS)

#include <HodEngine/Core/Output.hpp>

#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalCommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterial.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalShader.hpp"
#include "HodEngine/Renderer/RHI/Metal/MetalTexture.hpp"

namespace hod
{
	namespace renderer
	{
		_SingletonOverrideConstructor(RendererMetal)
		: Renderer()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererMetal::~RendererMetal()
		{

		}

		bool RendererMetal::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount)
		{
			return false;
		}

		CommandBuffer* RendererMetal::CreateCommandBuffer()
		{
			return nullptr;
		}

		Buffer* RendererMetal::CreateBuffer(Buffer::Usage usage)
		{
			return nullptr;
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
		bool RendererMetal::ResizeSwapChain()
		{
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererMetal::AcquireNextImageIndex()
		{
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool RendererMetal::SwapBuffer()
		{
			return false;
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
			return new MetalMaterial();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* RendererMetal::CreateMaterialInstance(const Material* material)
		{
			return new MetalMaterialInstance(*material);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Texture* RendererMetal::CreateTexture()
		{
			return new MetalTexture();
		}
	}
}

#endif
