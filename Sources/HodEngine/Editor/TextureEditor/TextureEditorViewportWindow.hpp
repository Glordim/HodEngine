#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"

#include <memory>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline rhi
{
	class RenderTarget;
	class Shader;
}

namespace hod::inline renderer
{
	class Material;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_EDITOR_API TextureEditorViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(TextureEditorViewportWindow, EditorTabWindow)

	public:

					TextureEditorViewportWindow() = default;
					TextureEditorViewportWindow(EditorTab* editorTab);
					~TextureEditorViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		bool		CreateMaterial();

	private:

		// The texture is drawn into it by the TexturePreview shader (checker, channels), then shown by ImGui
		RenderTarget*	_renderTarget = nullptr;

		ImVec2			_zoomAnchor = ImVec2(0.0f, 0.0f);
		float			_drawnZoomFactor = 0.0f; // the zoom of the last frame drawn, 0 before: tells a zoom changed out of the viewport // where the cursor was at the last wheel notch, from the top left of the viewport: what the zoom is around

		Shader*			_vertexShader = nullptr;
		Shader*			_fragmentShader = nullptr;
		Material*		_material = nullptr;
	};
}
