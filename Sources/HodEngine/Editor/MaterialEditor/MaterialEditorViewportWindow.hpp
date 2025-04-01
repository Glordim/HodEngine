#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"

#include "HodEngine/Renderer/RenderQueue.hpp"

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
	class HOD_EDITOR_API MaterialEditorViewportWindow : public EditorTabWindow
	{
		META_TYPE(MaterialEditorViewportWindow, EditorTabWindow)
		WINDOW_DESCRIPTION()

	public:

					MaterialEditorViewportWindow() = default;
					MaterialEditorViewportWindow(EditorTab* editorTab);
					~MaterialEditorViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		renderer::RenderTarget* 	_renderTarget = nullptr;
		renderer::RenderQueue		_renderQueue;

		Matrix4						_projection;
		Matrix4						_view;

		float						_size = 5.0f;
	};
}
