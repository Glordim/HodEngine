#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/Application.hpp"
#include "HodEngine/Application/InitGuard.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/GameSystems/Job/JobScheduler.hpp>

#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include "HodEngine/UI/Builtin.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include <HodEngine/Game/BootInfo.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneResource.hpp>
#include <HodEngine/Game/World.hpp>

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
		return true;
	}

	bool Application::TerminateGameSystems()
	{
		ResourceManager::DestroyInstance();
		FrameSequencer::DestroyInstance();
		JobScheduler::DestroyInstance();
		return true;
	}

	bool Application::InitPhysics()
	{
		if (Physics::CreatePhysicsInstance()->Init() == false)
		{
			return false;
		}
		return true;
	}

	bool Application::TerminatePhysics()
	{
		Physics::DestroyInstance();
		return true;
	}

	bool Application::InitGame()
	{
		ComponentFactory::CreateInstance();
		SerializedDataFactory::CreateInstance();
		game::RegisterBuiltin();
		ui::RegisterBuiltin();
		return true;
	}

	bool Application::BootGame()
	{
		hod::Path buildPath = FileSystem::GetExecutablePath();
		/*
		const hod::Argument* datasPathArgument = argumentParser.GetArgument('p', "BuildPath");
		if (datasPathArgument != nullptr && datasPathArgument->_values[0] != nullptr)
		{
			buildPath = datasPathArgument->_values[0];
		}
		*/

		Document           document;
		DocumentReaderJson reader;
		reader.Read(document, buildPath / "Boot.json");

		BootInfo bootInfo;
		Serializer::Deserialize(bootInfo, document.GetRootNode());

		ResourceManager::GetInstance()->SetResourceDirectory(buildPath / "Datas");

	#if defined(HOD_GAME_MODULE_STATIC)
		StartupModule();
	#else
		_gameModule.Init(buildPath / bootInfo._gameModule, false);
		if (_gameModule.Load() == false)
		{
			return false;
		}
		auto startupFunc = _gameModule.LoadFunction<int(*)()>("StartupModule");
		if (startupFunc == nullptr)
		{
			return false;
		}
		startupFunc();
	#endif

		std::shared_ptr<hod::SceneResource> sceneResource = hod::ResourceManager::GetInstance()->GetResource<hod::SceneResource>(bootInfo._startupScene);
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

		return true;
	}

	bool Application::TerminateGame()
	{
		hod::DefaultAllocator::GetInstance().Delete(_world);
		_world = nullptr;

	#if defined(HOD_GAME_MODULE_STATIC)
		ShutdownModule();
	#else
		auto shutdownFunc = _gameModule.LoadFunction<int(*)()>("ShutdownModule");
		if (shutdownFunc != nullptr)
		{
			shutdownFunc();
		}
	#endif

		SerializedDataFactory::DestroyInstance();
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
		if (!initGuard.Push([this]{ return InitPhysics(); },     [this]{ TerminatePhysics(); }))     return false;
		if (!initGuard.Push([this]{ return InitGame(); },        [this]{ TerminateGame(); }))        return false;

		if (BootGame() == false)
		{
			return false;
		}

		FrameSequencer* frameSequencer = FrameSequencer::GetInstance();

		constexpr double targetTimeStep = (1.0 / 60.0) * 1000.0;

		SystemTime::TimeStamp last = SystemTime::Now();
		while (_shouldQuit == false)
		{
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
