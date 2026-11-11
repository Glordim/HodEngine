#pragma once
#include "HodEngine/GameEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include "HodEngine/Editor/AssetDatabase.hpp"

#include <HodEngine/Core/String.hpp>

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

		// Add Component popup: the search, the folder browsed while there is none (a category path, empty at the root), the
		// highlighted row (the one Enter activates, moved with the arrows) among the shown ones
		char		_addComponentSearch[256] = {'\0'};
		String		_addComponentFolder;
		String		_addComponentHighlightFolder; // the folder row to highlight on the next draw (the one left by going up)
		int32_t		_addComponentHighlight = 0;
		int32_t		_addComponentMove = 0; // arrows pressed in the search field this frame
		bool		_addComponentResetHighlight = false; // on the first row (not the way back) on the next draw
		bool		_addComponentScrollToHighlight = false;
		bool		_addComponentFocusSearch = false; // given back to the search field on the next draw (after a click in the list...)
		int32_t		_addComponentSlideDirection = 0; // the rows sliding in after a folder change: from the right (1), the left (-1), or not (0)
		double		_addComponentSlideStart = 0.0; // ImGui time
		String		_addComponentPreviousFolder; // the folder left, sliding out
		int32_t		_addComponentPreviousHighlight = -1; // its row that was activated, still highlighted while it slides out
	};
}
