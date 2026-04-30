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

namespace hod::inline application
{
	bool Application::InitCore()
	{
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

	bool Application::TerminateGame()
	{
		SerializedDataFactory::DestroyInstance();
		ComponentFactory::DestroyInstance();
		return true;
	}

	/// @brief
	/// @return
	bool Application::RunInternal()
	{
		ThisThread::SetName("MainThread");

		InitGuard initGuard;

		if (!initGuard.Push([this]{ return InitCore(); },        [this]{ TerminateCore(); }))        return false;
		if (!initGuard.Push([this]{ return InitGameSystems(); }, [this]{ TerminateGameSystems(); })) return false;
		if (!initGuard.Push([this]{ return InitPhysics(); },     [this]{ TerminatePhysics(); }))     return false;
		if (!initGuard.Push([this]{ return InitGame(); },        [this]{ TerminateGame(); }))        return false;

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
