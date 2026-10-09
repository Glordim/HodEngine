#pragma once
#include "HodEngine/RHI/Export.hpp"

#if defined(PLATFORM_MACOS)

	#include "HodEngine/RHI/RhiDevice.hpp"

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

namespace hod::inline rhi
{
	class MetalContext;
	class MetalDevice;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API RhiDeviceMetal : public RhiDevice
	{
		_SingletonOverride(RhiDeviceMetal)

	protected:
		~RhiDeviceMetal() override;

	public:
		bool CreateContext(Window* window); // TODO virtual in Renderer ?

		bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		PresentationSurface* CreatePresentationSurface(window::Window* window) override;

		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr,
									const Semaphore* waitSemaphore = nullptr, const Fence* fence = nullptr) override;

		CommandBuffer*    CreateCommandBuffer() override;
		Buffer*           CreateBuffer(Buffer::Usage usage, uint32_t size) override;
		Shader*           CreateShader(Shader::ShaderType type) override;
		GraphicsPipeline*         CreateGraphicsPipeline(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
											GraphicsPipeline::PolygonMode polygonMode = GraphicsPipeline::PolygonMode::Fill, GraphicsPipeline::Topololy topololy = GraphicsPipeline::Topololy::TRIANGLE,
											bool useDepth = true) override;
		LegacyMaterialInstance* CreateLegacyMaterialInstance(const GraphicsPipeline* graphicsPipeline) override;
		Texture*          CreateTexture() override;
		RenderTarget*     CreateRenderTarget() override;
		Semaphore*        CreateSemaphore() override;
		Fence*            CreateFence() override;
		BindGroup*        CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
		                                  const Texture* const* textures, uint32_t textureCount) override;
		uint32_t          GetUniformBufferOffsetAlignment() const override;

		MTL::Device*        GetDevice() const;
		MTL4::CommandQueue* GetCommandQueue() const;
		MTL4::CommandAllocator* GetCommandAllocator(uint32_t frameIndex) const;

		void AddResourceToResidencySet(const MTL::Allocation* allocation);
		void RemoveResourceFromResidencySet(const MTL::Allocation* allocation);

	protected:
		bool InitDevice(uint32_t physicalDeviceIdentifier) override;
		void FlushDeferredDeletions(uint32_t frameIndex) override;

	private:
		MTL::Device*                    _device = nullptr;
		MTL4::CommandQueue*             _commandQueue = nullptr;
		Vector<MTL4::CommandAllocator*> _commandAllocators;
		MTL::ResidencySet*              _residencySet = nullptr;
	};
}

#endif
