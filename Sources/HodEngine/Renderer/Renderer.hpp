#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

#include <HodEngine/Core/Singleton.hpp>

#undef CreateSemaphore

namespace hod::application
{
	class Application;
}

namespace hod::window
{
	class Window;
}

namespace hod::renderer
{
	struct GpuDevice;
	class RenderView;
	class Buffer;
	class CommandBuffer;
	class Material;
	class MaterialInstance;
	class Texture;
	class Context;
	class VertexInput;
	class RenderTarget;
	class Semaphore;
	class Fence;
	class FrameResources;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API Renderer
	{
		_SingletonAbstract(Renderer)

	public:
		enum VisualizationMode
		{
			Normal = 0,
			NormalWithWireframe,
			Wireframe,
			Overdraw,
			Count
		};

	public:
		virtual ~Renderer();

		virtual bool Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) = 0;
		virtual void Clear();

		virtual bool BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier = 0) = 0;

		virtual bool GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) = 0;

		virtual bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr,
		                                  const Semaphore* waitSemaphore = nullptr, const Fence* fence = nullptr) = 0;

		virtual CommandBuffer* CreateCommandBuffer() = 0;
		virtual Buffer*        CreateBuffer(Buffer::Usage usage, uint32_t size) = 0;
		virtual Semaphore*     CreateSemaphore() = 0;
		virtual Fence*         CreateFence() = 0;

		virtual Shader*           CreateShader(Shader::ShaderType type) = 0;
		virtual Material*         CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader,
		                                         Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE,
		                                         bool useDepth = true) = 0;
		virtual MaterialInstance* CreateMaterialInstance(const Material* material) = 0;
		virtual Texture*          CreateTexture() = 0;
		virtual RenderTarget*     CreateRenderTarget() = 0;

		FrameResources& GetCurrentFrameResources();

		void PushRenderView(RenderView& renderView, bool autoDestroyAfterFrame = true);
		void RenderViews();
		void WaitViews();

		void Render();

		// Debug
	public:
		VisualizationMode GetVisualizationMode() const;
		void              SetVisualizationMode(VisualizationMode visualizationMode);

		MaterialInstance* GetDefaultMaterialInstance();
		MaterialInstance* GetOverdrawMaterialInstance();
		MaterialInstance* GetWireframeMaterialInstance();

		Texture* GetDefaultWhiteTexture();

	protected:
		Material*         _overdrawnMaterial = nullptr;
		MaterialInstance* _overdrawnMaterialInstance = nullptr;

		Material*         _wireframeMaterial = nullptr;
		MaterialInstance* _wireframeMaterialInstance = nullptr;

		Material*         _defaultMaterial = nullptr;
		MaterialInstance* _defaultMaterialInstance = nullptr;

		Vector<RenderView*> _renderViews;

		/*
		Material* _unlitVertexColorMaterial = nullptr;
		MaterialInstance* _unlitVertexColorMaterialInstance = nullptr;

		Material* _unlitVertexColorLineMaterial = nullptr;
		MaterialInstance* _unlitVertexColorLineMaterialInstance = nullptr;

		Material* _sharedMinimalMaterial = nullptr;
		*/

		VisualizationMode _visualizationMode = VisualizationMode::Normal;

		Texture* _defaultWhiteTexture = nullptr;

		// FIF
		uint32_t               _frameCount = 0;
		uint32_t               _frameIndex = 0;
		const uint32_t         _frameInFlight = 2;
		Vector<FrameResources> _frameResources;
		//
	};
}
