#pragma once
#include "HodEngine/GameEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"

#include <memory>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief
	class HOD_GAME_EDITOR_API DataItemEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(DataItemEditorPropertiesWindow, EditorTabWindow)

	public:

					DataItemEditorPropertiesWindow() = default;
					DataItemEditorPropertiesWindow(EditorTab* editorTab);
					~DataItemEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;
	};
}
