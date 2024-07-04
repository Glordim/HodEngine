#include "HodEngine/Application/Application.hpp"

#include "HodEngine/Core/FileSystem.hpp"

#include <HodEngine/Core/Job/JobScheduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"
#include "HodEngine/Core/ResourceManager.hpp"

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

		physics::Physics::CreateInstance()->Init();

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

	bool Application::LoadDll()
	{
#if defined(PLATFORM_WINDOWS)
		std::filesystem::path exeDirPath = FileSystem::GetExecutablePath().parent_path();
		std::filesystem::copy_file(exeDirPath / "Pong.dll", exeDirPath / "Pong-Loaded.dll", std::filesystem::copy_options::update_existing);

		_dll = LoadLibrary((exeDirPath / "Pong-Loaded.dll").string().c_str());
		if (_dll == NULL)
		{
			//std::cout << "could not load the dynamic library" << std::endl;
			return false;
		}

		using initFunction = int(*)();

		// resolve function address here
		initFunction initFunc = (initFunction)GetProcAddress(_dll, "Init");
		if (initFunc == nullptr)
		{
			//std::cout << "could not locate the function" << std::endl;
			return false;
		}

		initFunc();
#endif
		return true;
	}

	bool Application::UnloadDll()
	{
#if defined(PLATFORM_WINDOWS)
		if (_dll)
		{
			using cleanFunction = int(*)();

			cleanFunction cleanFunc = (cleanFunction)GetProcAddress(_dll, "Clean");
			if (cleanFunc == nullptr)
			{
				//std::cout << "could not locate the function" << std::endl;
				return false;
			}

			cleanFunc();
		}

		FreeLibrary(_dll);
		_dll = NULL;

		std::filesystem::path exeDirPath = FileSystem::GetExecutablePath().parent_path();
		std::filesystem::remove((exeDirPath / "Pong-Loaded.dll").string().c_str());
#endif
		return true;
	}

	/// @brief 
	/// @return 
	bool Application::ReloadDll()
	{
		if (UnloadDll() == false)
		{
			return false;
		}
		if (LoadDll() == false)
		{
			return false;
		}
		return true;
	}
}
