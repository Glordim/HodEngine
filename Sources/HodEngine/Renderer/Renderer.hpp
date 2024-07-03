#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>
#include <vector>

#include "HodEngine/Renderer/RHI/Shader.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RenderQueue.hpp"

#include <HodEngine/Core/Singleton.hpp>

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
	class RenderTarget;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_API Renderer
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
		virtual Buffer*				CreateBuffer(Buffer::Usage usage, uint32_t size) = 0;
		virtual Shader*				CreateShader(Shader::ShaderType type) = 0;
		virtual Material*			CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) = 0;
		virtual MaterialInstance*	CreateMaterialInstance(const Material* material) = 0;
		virtual Texture*			CreateTexture() = 0;
		virtual RenderTarget*		CreateRenderTarget() = 0;

		//Debug
	public:

		VisualizationMode			GetVisualizationMode() const;
		void						SetVisualizationMode(VisualizationMode visualizationMode);

		MaterialInstance*			GetDefaultMaterialInstance();
		MaterialInstance*			GetOverdrawMaterialInstance();
		MaterialInstance*			GetWireframeMaterialInstance();

		Texture*					GetDefaultWhiteTexture();

	private:

		Material*					_overdrawnMaterial = nullptr;
		MaterialInstance*			_overdrawnMaterialInstance = nullptr;

		Material*					_wireframeMaterial = nullptr;
		MaterialInstance*			_wireframeMaterialInstance = nullptr;

		Material*					_defaultMaterial = nullptr;
		MaterialInstance*			_defaultMaterialInstance = nullptr;

		/*
		Material* _unlitVertexColorMaterial = nullptr;
		MaterialInstance* _unlitVertexColorMaterialInstance = nullptr;

		Material* _unlitVertexColorLineMaterial = nullptr;
		MaterialInstance* _unlitVertexColorLineMaterialInstance = nullptr;

		Material* _sharedMinimalMaterial = nullptr;
		*/

		VisualizationMode			_visualizationMode = VisualizationMode::Normal;

		RenderQueue					_renderQueue;

		Texture*					_defaultWhiteTexture = nullptr;
	};
}
