#include "HodEngine/Application/Application.h"

#include <HodEngine/Core/Job/JobSqueduler.h>
#include <HodEngine/Core/Frame/FrameSequencer.h>

#include "HodEngine/Game/World.h"
#include "HodEngine/Game/Builtin.h"
#include "HodEngine/Game/ComponentFactory.h"

namespace hod::application
{
	_SingletonConstructor(Application)
	{

	}

	/// @brief 
	/// @param argc 
	/// @param argv 
	/// @return 
	bool Application::Init(const core::ArgumentParser& argumentParser)
	{
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
