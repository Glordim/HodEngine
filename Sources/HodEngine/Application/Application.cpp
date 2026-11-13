#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/Application.hpp"
#include "HodEngine/Application/InitGuard.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/GameSystems/Job/JobScheduler.hpp>
#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>

#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include <HodEngine/Core/RuntimeModule.hpp>

#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/Thread/Thread.hpp>

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneResource.hpp>
#include <HodEngine/Game/World.hpp>

#if defined(HOD_APPLICATION_STATIC)
extern "C"
{
	// Generated in the game (HodModules.generated.cpp): starts the runtime modules of the plugins the project enables, dependencies
	// first, then the game module
	bool HodStartRuntimeModules(hod::core::ModuleHost& host);
}
#endif

namespace hod::inline application
{
	bool Application::InitCore()
	{
		ThisThread::SetName("MainThread");

		FileSystemConfig config;
		ConfigureFileSystem(config);
	#if defined(PLATFORM_ANDROID)
		if (FileSystem::CreateInstance()->Init(config._assetManager) == false)
	#else
		if (FileSystem::CreateInstance()->Init() == false)
	#endif
		{
			return false;
		}
		return true;
	}

	bool Application::TerminateCore()
	{
		FileSystem::DestroyInstance();
		return true;
	}

	bool Application::InitGameSystems()
	{
		JobScheduler::CreateInstance();
		FrameSequencer::CreateInstance();
		ResourceManager::CreateInstance();
		ProjectSettingsManager::CreateInstance();
		return true;
	}

	bool Application::TerminateGameSystems()
	{
		ProjectSettingsManager::DestroyInstance();
		ResourceManager::DestroyInstance();
		FrameSequencer::DestroyInstance();
		JobScheduler::DestroyInstance();
		return true;
	}

	/// @brief The project settings loaded (the modules may read theirs), then the startup of the runtime modules, in their order
	/// @return false if one failed: the ones started already are stopped
	bool Application::InitModules()
	{
		_runtimeModuleHost = DefaultAllocator::GetInstance().New<ModuleHost>();
#if defined(HOD_APPLICATION_STATIC)
		hod::Path buildPath = FileSystem::GetExecutablePath().ParentPath();
		ProjectSettingsManager::GetInstance()->Load(buildPath / "Datas" / "ProjectSettings");
		if (HodStartRuntimeModules(*_runtimeModuleHost) == false)
		{
			DefaultAllocator::GetInstance().Delete(_runtimeModuleHost);
			_runtimeModuleHost = nullptr;
			return false;
		}
#endif
		return true;
	}

	/// @brief Each runtime module stopped, in the reverse order
	/// @return
	bool Application::TerminateModules()
	{
		DefaultAllocator::GetInstance().Delete(_runtimeModuleHost);
		_runtimeModuleHost = nullptr;
		return true;
	}

	bool Application::InitGame()
	{
		ComponentFactory::CreateInstance();
		DataStructFactory::CreateInstance();
		game::RegisterBuiltin();
		return true;
	}

	bool Application::BootGame()
	{
#if defined(HOD_APPLICATION_STATIC)
		hod::Path buildPath = FileSystem::GetExecutablePath().ParentPath();
		/*
		const hod::Argument* datasPathArgument = argumentParser.GetArgument('p', "BuildPath");
		if (datasPathArgument != nullptr && datasPathArgument->_values[0] != nullptr)
		{
			buildPath = datasPathArgument->_values[0];
		}
		*/

		ResourceManager::GetInstance()->SetResourceDirectory(buildPath / "Datas");

		UID startupSceneUID(5697855804628231253ull, 12087004502242050213ull);// = GameInfo::GetStartupScene();
		std::shared_ptr<hod::SceneResource> sceneResource = hod::ResourceManager::GetInstance()->GetResource<hod::SceneResource>(startupSceneUID);
		if (sceneResource == nullptr)
		{
			return false;
		}

		hod::Scene* startupScene = sceneResource->CreateScene();
		if (startupScene == nullptr)
		{
			return false;
		}

		_world = hod::DefaultAllocator::GetInstance().New<hod::game::World>();
		_world->Init();
		_world->AddScene(startupScene);
		_world->SetEditorPlaying(true);
		startupScene->ProcessActivation();
#endif
		return true;
	}

	/// @brief Before the runtime modules shut down: the world's systems and components use them
	/// @return
	bool Application::TerminateBoot()
	{
		hod::DefaultAllocator::GetInstance().Delete(_world);
		_world = nullptr;
		return true;
	}

	bool Application::TerminateGame()
	{
		DataStructFactory::DestroyInstance();
		ComponentFactory::DestroyInstance();
		return true;
	}

	/// @brief
	/// @return
	bool Application::RunInternal()
	{
		InitGuard initGuard;

		if (!initGuard.Push([this]{ return InitCore(); },        [this]{ TerminateCore(); }))        return false;
		if (!initGuard.Push([this]{ return InitGameSystems(); }, [this]{ TerminateGameSystems(); })) return false;
		if (!initGuard.Push([this]{ return InitGame(); },        [this]{ TerminateGame(); }))        return false;
		if (!initGuard.Push([this]{ return InitModules(); },     [this]{ TerminateModules(); }))     return false;
		if (!initGuard.Push([this]{ return BootGame(); },        [this]{ TerminateBoot(); }))        return false;

		FrameSequencer* frameSequencer = FrameSequencer::GetInstance();

		constexpr double targetTimeStep = (1.0 / 60.0) * 1000.0;

		SystemTime::TimeStamp last = SystemTime::Now();
		while (_shouldQuit == false)
		{
			JobScheduler::GetInstance()->CleanupCompleted();
			frameSequencer->EnqueueAndWaitJobs();

			SystemTime::TimeStamp now = SystemTime::Now();
			double                elapsedTime = SystemTime::ElapsedTimeInMilliseconds(last, now);
			last = now;

			double sleepTime = targetTimeStep - elapsedTime;
			if (sleepTime > 0.0)
			{
				ThisThread::Sleep(static_cast<uint32_t>(sleepTime));
			}
		}

		return true;
	}

	/// @brief
	void Application::Quit()
	{
		_shouldQuit = true;
	}
}
