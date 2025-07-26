#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"

#include <memory>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	class Actor;
	class Entity;
}

namespace hod::editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_EDITOR_API InspectorWindow : public EditorTabWindow
	{
		META_TYPE(InspectorWindow, EditorTabWindow)
		WINDOW_DESCRIPTION()

	public:

					InspectorWindow() = default;
					InspectorWindow(EditorTab* editorTab);
					~InspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawAssetSelection(const AssetDatabase::FileSystemMapping* selection);
		void		DrawSceneSelection(game::Entity* selection);

		bool		DrawDefaultInspector(EditorReflectedObject& reflectedObject);
	};
}
