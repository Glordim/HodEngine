#pragma once
#include <HodEngine/HodEngine.hpp>

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
	class HOD_API InspectorWindow : public imgui::Window
	{
		META_TYPE(InspectorWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					InspectorWindow() = default;
					~InspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		void		DrawAssetSelection(const AssetDatabase::FileSystemMapping* selection);
		void		DrawSceneSelection(std::shared_ptr<game::Entity> selection);

		bool		DrawDefaultInspector(EditorReflectedObject& reflectedObject);
	};
}
