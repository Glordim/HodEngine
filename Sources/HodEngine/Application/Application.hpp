#pragma once
#include "HodEngine/Application/Export.hpp"

#if defined(PLATFORM_ANDROID)
struct AAssetManager;
#endif

namespace hod::inline core
{
	class RuntimeModuleHost;
}

namespace hod::inline game
{
	class World;
}

namespace hod::inline application
{
	/// @brief
	class HOD_APPLICATION_API Application
	{
	public:
		virtual ~Application() = default;

		bool CheckIfGameSharedLibraryExist() const;

		void Quit();
		
		protected:
		
		virtual bool RunInternal();
		
		// Core
		struct FileSystemConfig
		{
			#if defined(PLATFORM_ANDROID)
			AAssetManager* _assetManager = nullptr;
			#endif
		};
		virtual void ConfigureFileSystem(FileSystemConfig& /*fileSystemConfig*/) {}
		
		bool InitCore();
		bool TerminateCore();
		
		// GameSystems
		bool InitGameSystems();
		bool TerminateGameSystems();
		
		// Game: the factories, the builtin types, then the startup of the game's runtime libraries (see RuntimeModuleHost)
		bool InitGame();
		bool TerminateGame();

		// The game's runtime libraries: the project settings are loaded, then their Init phase; undone (services, then types) in reverse
		bool InitModules();
		bool TerminateModules();

		// The game module, then its startup scene
		bool BootGame();
		bool TerminateBoot();

	protected:
		bool _shouldQuit = false;

		World*	_world = nullptr;

		RuntimeModuleHost* _runtimeModuleHost = nullptr;
	};
}
