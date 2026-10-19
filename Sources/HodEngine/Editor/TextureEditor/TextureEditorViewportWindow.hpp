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

		Shader*			_vertexShader = nullptr;
		Shader*			_fragmentShader = nullptr;
		Material*		_material = nullptr;
	};
}
