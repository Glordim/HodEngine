#pragma once
#include "HodEngine/Application/Export.hpp"

#include <HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp>

#if defined(PLATFORM_ANDROID)
struct AAssetManager;
#endif

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
		
		// Physics
		bool InitPhysics();
		bool TerminatePhysics();
		
		// Game
		bool InitGame();
		bool TerminateGame();
		bool BootGame();

	protected:
		bool _shouldQuit = false;

		DynamicLibrary	_gameModule;

		World*	_world = nullptr;
	};
}
