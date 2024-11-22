#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/Application.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/Core/Job/JobScheduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"

#include <thread>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace hod::application
{
	_SingletonConstructor(Application)
	{

	}

	/// @brief 
	/// @param argc 
	/// @param argv 
	/// @return 
	bool Application::Init(const ArgumentParser& argumentParser)
	{
		FileSystem::SetWorkingDirectory(FileSystem::GetExecutablePath().parent_path() / "Data");

		JobScheduler::CreateInstance();
		FrameSequencer::CreateInstance();

		ResourceManager::CreateInstance();

		physics::Physics::CreatePhysicsInstance()->Init();

		game::ComponentFactory::CreateInstance();
		game::SerializedDataFactory::CreateInstance();
		game::RegisterBuiltin();

		game::World::CreateInstance()->Init();

		return true;
	}

	/// @brief 
	void Application::Terminate()
	{
		game::World::DestroyInstance();
		game::SerializedDataFactory::DestroyInstance();
		game::ComponentFactory::DestroyInstance();

		physics::Physics::DestroyPhysicsInstance();

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
			double elapsedTime = SystemTime::ElapsedTimeInMilliseconds(last, now);
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
