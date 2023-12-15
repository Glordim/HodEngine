#include "HodEngine/Application/Application.hpp"

#include "HodEngine/Core/FileSystem.hpp"

#include <HodEngine/Core/Job/JobSqueduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

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

		game::World::CreateInstance();

		return true;
	}

	/// @brief 
	/// @return 
	bool Application::Run()
	{
		FrameSequencer* frameSequencer = FrameSequencer::GetInstance();

		while (_shouldQuit == false)
		{
			frameSequencer->EnqueueAndWaitJobs();
		}

		return true;
	}

	/// @brief 
	void Application::Quit()
	{
		_shouldQuit = true;
	}
}
