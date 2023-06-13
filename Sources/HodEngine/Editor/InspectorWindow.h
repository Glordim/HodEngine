#pragma once

#include <HodEngine/ImGui/Window.h>

#include "HodEngine/Editor/AssetDatabase.h"

namespace hod
{
	class Object;
}

namespace hod::game
{
	class Actor;
}

namespace hod::editor
{
	/// @brief 
	class InspectorWindow : public imgui::Window
	{
		META_TYPE(InspectorWindow);
		WINDOW_DESCRIPTION()

	public:

					InspectorWindow() = default;
					~InspectorWindow() override = default;

	public:

		void		Draw() override;

	private:

		void		DrawAssetSelection(const AssetDatabase::FileSystemMapping* selection);
		void		DrawSceneSelection(game::Actor* selection);

		bool		DrawDefaultInspector(hod::Object& object);
	};
}
