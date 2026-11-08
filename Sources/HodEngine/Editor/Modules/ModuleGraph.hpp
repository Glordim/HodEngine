#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline editor
{
	/// @brief Describes an engine module (a feature: its runtime library and/or its editor library), from its HodModule.json, copied by the
	/// build next to the editor in Modules/<Name>.json (see hod_module_manifest in Global.cmake)
	struct HOD_EDITOR_API ModuleManifest
	{
		REFLECTED_CLASS_NO_VIRTUAL(ModuleManifest)

	public:
		String _name;
		String _description;

		/// @brief The library (CMake target HodEngine::<_runtimeLibrary>) of an optional module, with a HOD_DEFINE_RUNTIME_MODULE: linked
		/// in the game only if the module is enabled, loaded by the editor. Empty for the modules always linked (Core, Game...)
		String _runtimeLibrary;

		/// @brief Its editor module (see HOD_DECLARE_EDITOR_MODULE), loaded after the ones of its dependencies. Optional
		String _editorLibrary;

		/// @brief Names of the modules it needs: enabled, loaded, initialized before it
		Vector<String> _dependencies;

		/// @brief What it implements ("Physics2d", "Audio"...): two enabled modules can't provide the same thing
		Vector<String> _provides;

		/// @brief Can't be disabled
		bool _required = false;

		/// @brief The names of the version control providers its editor library registers ("Git"), known before it's loaded: offered
		/// when a project is created (see NewProjectForm)
		Vector<String> _versionControlSystems;

	public:
		/// @return false (with an error) if the file can't be read or has no name
		static bool Load(const Path& path, ModuleManifest& manifest);
	};

	/// @brief The engine modules: validated, sorted (dependencies first, then by name: the same order everywhere, the editor's and the
	/// games'), then resolved against what a project disables
	class HOD_EDITOR_API ModuleGraph
	{
	public:
		struct Module
		{
			ModuleManifest _manifest;
			bool           _enabled = true;
			String         _disabledReason; // Why it isn't enabled, for the users
		};

	public:
		/// @brief The *.json files of the directory
		/// @return false (with an error) if one couldn't be read (the others are given)
		static bool LoadManifests(const Path& directory, Vector<ModuleManifest>& manifests);

		/// @brief Validates and sorts. A module with the name of another, or that needs one that doesn't exist, or that is in a dependency
		/// cycle, is left out (with an error), the ones needing it too. Then everything is enabled (see Resolve)
		/// @return false if one was left out
		bool Build(const Vector<ModuleManifest>& manifests);

		/// @brief Disables the modules named (unknown ones ignored, required ones refused, with a warning), then the second of two enabled
		/// modules that provide the same thing (with an error), then the modules that need a disabled one
		void Resolve(const Vector<String>& disabledModules);

		/// @brief Sorted: dependencies first, then by name
		const Vector<Module>& GetModules() const;
		const Module*         Find(const std::string_view& name) const;

		/// @brief The _runtimeLibrary of the enabled modules that have one, in the modules' order
		Vector<String> GetEnabledRuntimeLibraries() const;

		/// @brief What a user asks for, in a list of disabled modules (Resolve's): enabling a module enables what it needs too, disabling
		/// it lets Resolve disable the ones needing it
		void SetEnabled(Vector<String>& disabledModules, const String& name, bool enabled) const;

		/// @brief What went wrong in Build and Resolve
		const Vector<String>& GetErrors() const;

	private:
		Vector<Module> _modules;
		Vector<String> _errors;
		uint32_t       _buildErrorCount = 0;
	};
}
