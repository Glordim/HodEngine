#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/Application.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/GameSystems/Job/JobScheduler.hpp>

#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"

#include <thread>

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

namespace hod::inline application
{
	_SingletonConstructor(Application) {}

	/// @brief
	/// @param argc
	/// @param argv
	/// @return
	bool Application::Init(const ArgumentParser& argumentParser)
	{
		ThisThread::SetName("MainThread");

		(void)argumentParser;

		FileSystem::SetWorkingDirectory(FileSystem::GetExecutablePath().ParentPath() / "Data");

		JobScheduler::CreateInstance();
		FrameSequencer::CreateInstance();

		ResourceManager::CreateInstance();

		Physics::CreatePhysicsInstance()->Init();

		ComponentFactory::CreateInstance();
		SerializedDataFactory::CreateInstance();
		RegisterBuiltin();

		return true;
	}

	/// @brief
	void Application::Terminate()
	{
		SerializedDataFactory::DestroyInstance();
		ComponentFactory::DestroyInstance();

		Physics::DestroyPhysicsInstance();

		ResourceManager::DestroyInstance();

		FrameSequencer::DestroyInstance();
		JobScheduler::DestroyInstance();

		FileSystem::DestroyInstance();
	}

	/// @brief
	/// @return
	bool Application::Run()
	{
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
