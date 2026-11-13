#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline editor
{
	/// @brief Describes an engine plugin (a feature: its runtime modules and/or its editor module, each a library with an entry point), from
	/// its HodPlugin.json, copied by the build next to the editor in Plugins/<Name>.json (see hod_plugin_manifest in Global.cmake)
	struct HOD_EDITOR_API PluginManifest
	{
		REFLECTED_CLASS_NO_VIRTUAL(PluginManifest)

	public:
		String _name;
		String _description;

		/// @brief Its runtime modules: the libraries (CMake targets HodEngine::<module>) with an entry point (HOD_STARTUP_RUNTIME_MODULE), in
		/// their order: its base, then its Components layer (Physics: the physics engine, PhysicsComponents: its components). Linked in the
		/// game only if the plugin is enabled, loaded by the editor. Empty for the plugins always linked (Core, Game...). A library without
		/// anything to start (Renderer: the application's) isn't listed
		Vector<String> _runtimeModules;

		/// @brief Its editor module (see HOD_STARTUP_EDITOR_MODULE), loaded after the ones of its dependencies. Optional
		String _editorModule;

		/// @brief Names of the plugins it needs: enabled, loaded, initialized before it
		Vector<String> _dependencies;

		/// @brief What it implements ("Physics2d", "Audio"...): two enabled plugins can't provide the same thing
		Vector<String> _provides;

		/// @brief Can't be disabled
		bool _required = false;

		/// @brief The names of the version control providers its editor module registers ("Git"), known before it's loaded: offered
		/// when a project is created (see NewProjectForm)
		Vector<String> _versionControlSystems;

	public:
		/// @return false (with an error) if the file can't be read or has no name
		static bool Load(const Path& path, PluginManifest& manifest);
	};

	/// @brief The engine plugins: validated, sorted (dependencies first, then by name: the same order everywhere, the editor's and the
	/// games'), then resolved against what a project disables
	class HOD_EDITOR_API PluginGraph
	{
	public:
		struct Plugin
		{
			PluginManifest _manifest;
			bool           _enabled = true;
			String         _disabledReason; // Why it isn't enabled, for the users
		};

	public:
		/// @brief The *.json files of the directory
		/// @return false (with an error) if one couldn't be read (the others are given)
		static bool LoadManifests(const Path& directory, Vector<PluginManifest>& manifests);

		/// @brief Validates and sorts. A plugin with the name of another, or that needs one that doesn't exist, or that is in a dependency
		/// cycle, is left out (with an error), the ones needing it too. Then everything is enabled (see Resolve)
		/// @return false if one was left out
		bool Build(const Vector<PluginManifest>& manifests);

		/// @brief Disables the plugins named (unknown ones ignored, required ones refused, with a warning), then the second of two enabled
		/// plugins that provide the same thing (with an error), then the plugins that need a disabled one
		void Resolve(const Vector<String>& disabledPlugins);

		/// @brief Sorted: dependencies first, then by name
		const Vector<Plugin>& GetPlugins() const;
		const Plugin*         Find(const std::string_view& name) const;

		/// @brief The _runtimeModules of the enabled plugins, in the plugins' order
		Vector<String> GetEnabledRuntimeModules() const;

		/// @brief What a user asks for, in a list of disabled plugins (Resolve's): enabling a plugin enables what it needs too, disabling
		/// it lets Resolve disable the ones needing it
		void SetEnabled(Vector<String>& disabledPlugins, const String& name, bool enabled) const;

		/// @brief What went wrong in Build and Resolve
		const Vector<String>& GetErrors() const;

	private:
		Vector<Plugin> _plugins;
		Vector<String> _errors;
		uint32_t       _buildErrorCount = 0;
	};
}
