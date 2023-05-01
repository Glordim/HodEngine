#pragma once

#include <string>
#include <vector>

#include "HodEngine/Renderer/RHI/Shader.h"
#include "HodEngine/Renderer/RHI/Material.h"
#include "HodEngine/Renderer/RHI/Buffer.h"
#include "HodEngine/Renderer/RenderQueue.h"

#include <HodEngine/Core/Singleton.h>

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
	class RenderQueue;
	class Buffer;
	class CommandBuffer;
	class Material;
	class MaterialInstance;
	class Texture;
	class Context;
	class VertexInput;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class Renderer : public Singleton<Renderer>
	{
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

		/*
		bool						Init();
		void						Clear();
		*/

		virtual bool				Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) = 0;
		virtual bool				BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier = 0) = 0;

		RenderQueue*				GetRenderQueue();

		virtual bool				GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) = 0;


		virtual bool				ResizeSwapChain() = 0;

		virtual bool				AcquireNextImageIndex() = 0;
		virtual bool				SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount) = 0;

		virtual bool				SwapBuffer() = 0;

		virtual CommandBuffer*		CreateCommandBuffer() = 0;
		virtual Buffer*				CreateBuffer(Buffer::Usage usage) = 0;
		virtual Shader*				CreateShader(Shader::ShaderType type) = 0;
		virtual Material*			CreateMaterial(VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) = 0;
		virtual MaterialInstance*	CreateMaterialInstance(const Material* material) = 0;
		virtual Texture*			CreateTexture() = 0;

		//Debug
	public:

		VisualizationMode			GetVisualizationMode() const;
		void						SetVisualizationMode(VisualizationMode visualizationMode);

		MaterialInstance*			GetOverdrawMaterialInstance();
		MaterialInstance*			GetWireframeMaterialInstance();

		Texture*					GetDefaultWhiteTexture();

	private:

		Material*					_overdrawnMaterial = nullptr;
		MaterialInstance*			_overdrawnMaterialInstance = nullptr;

		Material*					_wireframeMaterial = nullptr;
		MaterialInstance*			_wireframeMaterialInstance = nullptr;

		VisualizationMode			_visualizationMode = VisualizationMode::Normal;

		RenderQueue					_renderQueue;

		Texture*					_defaultWhiteTexture = nullptr;
	};
}