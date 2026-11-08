#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline editor
{
	/// @brief Edit > Plugins: the engine modules (see ModuleGraph), enabled or not for the project. The changes are pending (their effect
	/// shown: the modules needing a disabled one, the conflicts) until applied: saved in the project (_disabledModules), then the editor
	/// restarts on it (the game module is rebuilt with the libraries of the enabled modules, see Project::CanLoadProjectModules).
	class HOD_EDITOR_API PluginsWindow : public imgui::Window
	{
		REFLECTED_CLASS(PluginsWindow, imgui::Window)

	public:
		/// @brief Opens the window, or focuses it when already open
		static void Open();

	public:
		PluginsWindow();

		void DrawContent() override;

	private:
		void SetEnabled(const String& name, bool enabled);
		bool HasChanges() const;
		void Apply(bool saveTabs);

	private:
		Vector<String> _disabledModules; // Pending, as the project's _disabledModules
	};
}
