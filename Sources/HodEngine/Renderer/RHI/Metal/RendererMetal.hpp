#pragma once

#if defined(PLATFORM_MACOS)

#include "HodEngine/Renderer/Renderer.hpp"

namespace hod
{
	namespace renderer
	{
		class MetalContext;
		class MetalDevice;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RendererMetal : public Renderer
		{
			_SingletonOverride(RendererMetal)

		protected:

			~RendererMetal() override;

		public:

			bool		Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) override;
			bool		BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier = 0) override;

			bool		CreateContext(window::Window* window); // TODO virtual in Renderer ?

			virtual bool GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;

			virtual bool ResizeSwapChain() override; // TODO remove

			virtual bool AcquireNextImageIndex() override;
			virtual bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount) override;
			virtual bool SwapBuffer() override;

			virtual CommandBuffer* CreateCommandBuffer() override;
			virtual Buffer* CreateBuffer(Buffer::Usage usage) override;
			virtual Shader* CreateShader(Shader::ShaderType type) override;
			virtual Material* CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
			virtual MaterialInstance* CreateMaterialInstance(const Material* material) override;
			virtual Texture* CreateTexture() override;
			virtual RenderTarget* CreateRenderTarget() override;

			MetalDevice*	GetDevice() const;

		private:

			MetalDevice*	_device = nullptr;
			MetalContext*	_context = nullptr;
		};
	}
}

#endif
