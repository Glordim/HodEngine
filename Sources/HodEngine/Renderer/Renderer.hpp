#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <HodEngine/Core/Singleton.hpp>

namespace hod::inline rhi
{
	class PresentationSurface;
	class Shader;
	class Texture;
}

namespace hod::inline renderer
{
	class Material;
	class MaterialInstance;
}

namespace hod::inline renderer
{
	class RenderView;
	class FrameResources;

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API Renderer
	{
		_Singleton(Renderer)

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
		~Renderer();

		bool Init(uint32_t physicalDeviceIdentifier = 0);
		void Clear();

		FrameResources& GetCurrentFrameResources();

		// void PushRenderView(RenderView& renderView, bool autoDestroyAfterFrame = true);
		// void RenderViews();
		// void WaitViews();

		void Render();

		bool AcquireNextFrame();

		// Surface acquired at the very start of each frame, which paces the frame on its vsync. None by default.
		// Must be reset before that surface is destroyed.
		void                 SetMainPresentationSurface(PresentationSurface* presentationSurface);
		PresentationSurface* GetMainPresentationSurface() const;

		// Debug
	public:
		VisualizationMode GetVisualizationMode() const;
		void              SetVisualizationMode(VisualizationMode visualizationMode);

		MaterialInstance* GetDefaultMaterialInstance();
		MaterialInstance* GetOverdrawMaterialInstance();
		MaterialInstance* GetWireframeMaterialInstance();

		// What a MaterialInstance draws for a texture set to null
		Texture* GetWhiteTexture();

	private:
		Material*         _overdrawnMaterial = nullptr;
		MaterialInstance* _overdrawnMaterialInstance = nullptr;

		Material*         _wireframeMaterial = nullptr;
		MaterialInstance* _wireframeMaterialInstance = nullptr;

		Material*         _defaultMaterial = nullptr;
		MaterialInstance* _defaultMaterialInstance = nullptr;
		Shader*           _defaultVertexShader = nullptr;
		Shader*           _defaultFragmentShader = nullptr;

		Vector<RenderView*> _renderViews;

		/*
		Material* _unlitVertexColorMaterial = nullptr;
		MaterialInstance* _unlitVertexColorMaterialInstance = nullptr;

		Material* _unlitVertexColorLineMaterial = nullptr;
		MaterialInstance* _unlitVertexColorLineMaterialInstance = nullptr;

		Material* _sharedMinimalMaterial = nullptr;
		*/

		VisualizationMode _visualizationMode = VisualizationMode::Normal;

		Texture* _whiteTexture = nullptr;

		Vector<FrameResources> _frameResources;

		PresentationSurface* _mainPresentationSurface = nullptr;
	};
}
