#pragma once
#include "HodEngine/Renderer/Export.hpp"

#if defined(PLATFORM_MACOS)

	#include "HodEngine/Renderer/Renderer.hpp"

namespace MTL
{
	class Device;
	class ResidencySet;
	class Allocation;
}

namespace MTL4
{
	class CommandQueue;
	class CommandAllocator;
}

namespace hod::inline renderer
{
	class MetalContext;
	class MetalDevice;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API RendererMetal : public Renderer
	{
		_SingletonOverride(RendererMetal)

	protected:
		~RendererMetal() override;

	public:
		bool Init(Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) override;

		bool CreateContext(Window* window); // TODO virtual in Renderer ?

		bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		PresentationSurface* CreatePresentationSurface(window::Window* window) override;

		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr,
									const Semaphore* waitSemaphore = nullptr, const Fence* fence = nullptr) override;

		CommandBuffer*    CreateCommandBuffer() override;
		Buffer*           CreateBuffer(Buffer::Usage usage, uint32_t size) override;
		Shader*           CreateShader(Shader::ShaderType type) override;
		Material*         CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
											Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE,
											bool useDepth = true) override;
		MaterialInstance* CreateMaterialInstance(const Material* material) override;
		Texture*          CreateTexture() override;
		RenderTarget*     CreateRenderTarget() override;
		Semaphore*        CreateSemaphore() override;
		Fence*            CreateFence() override;

		MTL::Device*        GetDevice() const;
		MTL4::CommandQueue* GetCommandQueue() const;
		MTL4::CommandAllocator* GetCommandAllocator(uint32_t frameIndex) const;

		void AddResourceToResidencySet(const MTL::Allocation* allocation);
		void RemoveResourceFromResidencySet(const MTL::Allocation* allocation);

	protected:
		void FlushDeferredDeletions(uint32_t frameIndex) override;

	private:
		MTL::Device*                    _device = nullptr;
		MTL4::CommandQueue*             _commandQueue = nullptr;
		Vector<MTL4::CommandAllocator*> _commandAllocators;
		MTL::ResidencySet*              _residencySet = nullptr;
	};
}

#endif
