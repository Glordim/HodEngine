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

namespace hod::editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_EDITOR_API MaterialInstanceEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(MaterialInstanceEditorPropertiesWindow, EditorTabWindow)

	public:

					MaterialInstanceEditorPropertiesWindow() = default;
					MaterialInstanceEditorPropertiesWindow(EditorTab* editorTab);
					~MaterialInstanceEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
