#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"
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
	class Buffer;
	class CommandBuffer;
	class GraphicsPipeline;
	class MaterialInstance;
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
		virtual MaterialInstance* CreateMaterialInstance(const GraphicsPipeline* graphicsPipeline) = 0;
		virtual Texture*          CreateTexture() = 0;
		virtual RenderTarget*     CreateRenderTarget() = 0;

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
		virtual void FlushDeferredDeletions(uint32_t) {}

	protected:
		Vector<PresentationSurface*> _presentationSurfaces;

		Texture* _fallbackTexture = nullptr;

		// FIF
		uint32_t       _frameCount = 0;
		uint32_t       _frameIndex = 0;
		const uint32_t _frameInFlight = 2;
		//
	};
}
