#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Modules/ModuleGraph.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline editor
{
	/// @brief The "New project" page of the ProjectBrowser: a location (the folder the project goes in) and a name, the project folder
	/// being <location>/<name> (its full path shown, checked as it's typed, see Project::CheckNewProjectDirectory), the version control
	/// to create its workspace with, the engine modules it uses (as Edit > Plugins)
	class HOD_EDITOR_API NewProjectForm
	{
	public:
		enum class Result
		{
			None,      // Still filled
			Cancelled, // Back to the project list
			Created,   // Created and opened: the browser is closed at the end of the frame
		};

	public:
		/// @brief Empty form, the location given (the one of the last project, for instance)
		void Reset(const Path& location);

		Result Draw();

		/// @brief From the top of the window to the bottom of the buttons, padding included, as of the last Draw: what the window
		/// needs (the modules' section is collapsed by default, an advanced use)
		float GetContentHeight() const;

	private:
		Path GetProjectDirectory() const;
		void DrawModules();

	private:
		char _name[128] = {};
		char _location[1024] = {};
		bool _focusName = false;

		ModuleGraph    _moduleGraph; // Of the engine, resolved against _disabledModules each frame
		Vector<String> _disabledModules;
		float          _contentHeight = 0.0f;
		bool           _resetModulesHeader = false; // Collapsed again on the next frame (ImGui keeps its state)

		Vector<String> _versionControlSystems; // Offered by the enabled modules (see ModuleManifest::_versionControlSystems)
		String         _versionControl;        // Empty: none

		String _creationError;
	};
}
