#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"

#include <HodEngine/Renderer/RenderView.hpp>

#include <HodEngine/Core/Math/Matrix4.hpp>

#include <memory>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::renderer
{
	class RenderTarget;
}

namespace hod::editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_EDITOR_API MaterialInstanceEditorViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(MaterialInstanceEditorViewportWindow, EditorTabWindow)

	public:

					MaterialInstanceEditorViewportWindow() = default;
					MaterialInstanceEditorViewportWindow(EditorTab* editorTab);
					~MaterialInstanceEditorViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		renderer::RenderTarget* 	_renderTarget = nullptr;
		renderer::RenderView		_renderView;

		Matrix4						_projection;
		Matrix4						_view;

		float						_size = 5.0f;
	};
}
