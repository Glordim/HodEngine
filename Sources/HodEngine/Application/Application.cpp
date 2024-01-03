#include "HodEngine/Application/Application.hpp"

#include "HodEngine/Core/FileSystem.hpp"

#include <HodEngine/Core/Job/JobSqueduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"

#include <thread>

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

		JobSqueduler::CreateInstance();
		FrameSequencer::CreateInstance();

		game::ComponentFactory::CreateInstance();
		game::RegisterBuiltin();

		game::World::CreateInstance()->Init();

		return true;
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
