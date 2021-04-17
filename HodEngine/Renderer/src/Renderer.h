#pragma once

#include <string>
#include <vector>

#include "Debug/RendererDebugWindow.h"

#include "RHI/Shader.h"
#include "RHI/Material.h"
#include "RHI/Buffer.h"
#include "RenderQueue.h"

#include <Core/Src/Singleton.h>

namespace HOD
{
	namespace APPLICATION
	{
		class Application;
	}

	namespace RENDERER
	{
		struct GpuDevice;
		class RenderQueue;
		class Buffer;
		class CommandBuffer;
		class Material;
		class MaterialInstance;
		class Texture;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Renderer : public CORE::Singleton<Renderer>
		{
			friend class CORE::Singleton<Renderer>;

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

			Renderer();
			virtual						~Renderer();

			bool						Init();
			void						Clear();

			virtual bool				Init(APPLICATION::Application* pApplication, bool enableValidationLayers) = 0;
			virtual bool				SetupImGui() = 0;

			RenderQueue*				GetRenderQueue();

			virtual bool				GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) = 0;

			virtual bool				BuildPipeline(GpuDevice* gpuDevice) = 0;

			virtual bool				ResizeSwapChain() = 0;

			virtual bool				AcquireNextImageIndex() = 0;
			virtual bool				SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount) = 0;

			virtual bool				SwapBuffer() = 0;

			virtual CommandBuffer*		CreateCommandBuffer() = 0;
			virtual Buffer*				CreateBuffer(Buffer::Usage usage) = 0;
			virtual Shader*				CreateShader(const std::string& path, Shader::ShaderType type) = 0;
			virtual Material*			CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) = 0;
			virtual MaterialInstance*	CreateMaterialInstance(const Material* material) = 0;
			virtual Texture*			CreateTexture(const std::string& path) = 0;

			//Debug
		public:

			VisualizationMode			GetVisualizationMode() const;
			void						SetVisualizationMode(VisualizationMode visualizationMode);

			MaterialInstance*			GetOverdrawMaterialInstance();
			MaterialInstance*			GetWireframeMaterialInstance();

		private:

			Material*					_overdrawnMaterial = nullptr;
			MaterialInstance*			_overdrawnMaterialInstance = nullptr;

			Material*					_wireframeMaterial = nullptr;
			MaterialInstance*			_wireframeMaterialInstance = nullptr;

			VisualizationMode			_visualizationMode = VisualizationMode::Normal;

			RendererDebugWindow			_rendererDebugWindow;

			RenderQueue					_renderQueue;
		};
	}
}