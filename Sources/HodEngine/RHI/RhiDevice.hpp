#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/BindGroup.hpp"
#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/RHI/Sampler.hpp"
#include "HodEngine/RHI/Shader.hpp"

#include <HodEngine/Core/Singleton.hpp>

#undef CreateSemaphore

namespace hod::inline window
{
	class Window;
}

namespace hod::inline rhi
{
	struct GpuDevice;
	class BindGroup;
	class Buffer;
	class CommandBuffer;
	class GraphicsPipeline;
	class Texture;
	class PresentationSurface;
	class VertexInput;
	class RenderTarget;
	class Semaphore;
	class Fence;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API RhiDevice
	{
		_SingletonAbstract(RhiDevice)

	public:
		virtual ~RhiDevice();

		bool         Init(uint32_t physicalDeviceIdentifier = 0);
		virtual void WaitIdle() {}
		virtual void Clear();

		virtual bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) = 0;

		virtual bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr,
		                                  const Semaphore* waitSemaphore = nullptr, const Fence* fence = nullptr) = 0;

		virtual CommandBuffer* CreateCommandBuffer() = 0;
		virtual Buffer*        CreateBuffer(Buffer::Usage usage, uint32_t size) = 0;
		virtual Semaphore*     CreateSemaphore() = 0;
		virtual Fence*         CreateFence() = 0;

		virtual Shader*           CreateShader(Shader::ShaderType type) = 0;
		virtual GraphicsPipeline*         CreateGraphicsPipeline(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
		                                         GraphicsPipeline::PolygonMode polygonMode = GraphicsPipeline::PolygonMode::Fill, GraphicsPipeline::Topololy topololy = GraphicsPipeline::Topololy::TRIANGLE,
		                                         bool useDepth = true) = 0;
		virtual Texture*          CreateTexture() = 0;
		virtual RenderTarget*     CreateRenderTarget() = 0;

		// uniformBuffers: one per uniform block of the set. textureBindings: one per texture block of the set.
		// Both in the order of the pipeline's ShaderSetDescriptor for that set.
		virtual BindGroup* CreateBindGroup(const GraphicsPipeline* graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount,
		                                   const BindGroup::TextureBinding* textureBindings, uint32_t textureBindingCount) = 0;

		// Samplers are shared: the same settings always give the same Sampler, which stays owned by the device
		const Sampler* GetSampler(const Sampler::CreateInfo& createInfo);

		// The offsets given to CommandBuffer::SetBindGroup must be multiples of this
		virtual uint32_t GetUniformBufferOffsetAlignment() const = 0;

		virtual PresentationSurface* CreatePresentationSurface(window::Window* window) = 0;
		void                         DestroyPresentationSurface(window::Window* window);
		PresentationSurface*         FindPresentationSurface(window::Window* window) const;

		// Valid texture bound to every texture slot the caller left unset, so that a shader never samples an unbound slot.
		Texture* GetFallbackTexture() const;

		void BeginFrame();
		void EndFrame();

		uint32_t GetFrameIndex() const;
		uint32_t GetFrameInFlightCount() const;

	protected:
		virtual bool InitDevice(uint32_t physicalDeviceIdentifier) = 0;
		virtual Sampler* CreateSampler(const Sampler::CreateInfo& createInfo) = 0;
		virtual void FlushDeferredDeletions(uint32_t) {}

	protected:
		Vector<PresentationSurface*> _presentationSurfaces;

		Texture* _fallbackTexture = nullptr;

		Vector<Sampler*> _samplers;

		// FIF
		uint32_t       _frameCount = 0;
		uint32_t       _frameIndex = 0;
		const uint32_t _frameInFlight = 2;
		//
	};
}
