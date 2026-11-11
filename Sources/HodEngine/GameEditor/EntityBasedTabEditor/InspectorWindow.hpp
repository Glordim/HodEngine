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

namespace hod::inline game
{
	class Actor;
	class Entity;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_GAME_EDITOR_API InspectorWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(InspectorWindow, EditorTabWindow)

	public:

					InspectorWindow() = default;
					InspectorWindow(EditorTab* editorTab);
					~InspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawAssetSelection(const AssetDatabase::FileSystemMapping* selection);
		void		DrawSceneSelection(Entity* selection);

		bool		DrawDefaultInspector(EditorReflectedObject& reflectedObject);
		void		DrawAddComponentPopup(Entity* selection);

	private:

		// Add Component popup: the search, the highlighted result (the one Enter adds, moved with the arrows) among the shown ones
		char		_addComponentSearch[256] = {'\0'};
		int32_t		_addComponentHighlight = 0;
		int32_t		_addComponentMove = 0; // arrows pressed in the search field this frame
		bool		_addComponentScrollToHighlight = false;
	};
}
