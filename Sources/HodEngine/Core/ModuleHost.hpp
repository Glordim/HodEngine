#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/ModuleRegistration.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

namespace hod::inline core
{
	class DynamicLibrary;
	class Path;

	/// @brief The entry point of a module (a library of a plugin), runtime or editor (see HOD_STARTUP_MODULE): registers through the registrars, starts its services, pushes on the registration what undoes them
	/// @return false (with an error): it couldn't start, what it registered already is undone
	using ModuleStartup = bool (*)(ModuleRegistration& registration);

	/// @brief Starts modules in order (their plugins' order: dependencies first) and stops them in the reverse order, each with its
	/// ModuleRegistration: Application for the runtime modules compiled in an application (Start), the editor for the modules of the project's plugins
	/// (one host for the runtime ones, one for the editor ones), the editor for the project's runtime module and editor module (Load: the host loads
	/// their library and unloads it once the module is stopped)
	class HOD_CORE_API ModuleHost
	{
	public:
		ModuleHost() = default;
		ModuleHost(const ModuleHost&) = delete;
		ModuleHost(ModuleHost&&) = delete;
		~ModuleHost();

		ModuleHost& operator=(const ModuleHost&) = delete;
		ModuleHost& operator=(ModuleHost&&) = delete;

	public:
		/// @brief A module linked in (an application build, see HodStartRuntimeModules): its startup, right away
		/// @return false (with an error naming the module) if it failed: what it registered is undone, it isn't kept
		bool Start(const String& name, ModuleStartup startup);

		/// @brief A module in its own library: the library loaded (path as DynamicLibrary::Init takes it: no prefix, no extension), its entry
		/// point found (HOD_MODULE_STARTUP_FUNCTION_NAME) and started. The library is the host's from then on, unloaded right after its module
		/// is stopped
		/// @param copyForReload Loads a copy of the library, the original can be rebuilt meanwhile (see DynamicLibrary::Init)
		/// @return false (with an error naming the module) if it can't be loaded, has no entry point or failed to start: nothing is kept
		bool Load(const String& name, const Path& path, bool copyForReload = false);

		/// @brief The last count modules started undone, in the reverse order (a plugin's, when its next one failed to start)
		void StopLast(uint32_t count);

		/// @brief Each module undone, in the reverse order (see ModuleRegistration)
		void StopAll();

		uint32_t GetModuleCount() const;

	private:
		struct Entry
		{
			String              _name;
			ModuleRegistration* _registration = nullptr;
			DynamicLibrary*     _library = nullptr; // Loaded by the host (see Load), unloaded once the module is stopped
		};
		Vector<Entry> _entries;
	};
}
