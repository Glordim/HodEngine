#include "Application.h"

#include <HodEngine/Core/Src/Job/JobSqueduler.h>
#include <HodEngine/Core/Src/Frame/FrameSequencer.h>

namespace HOD
{
	_SingletonInstance(Application)

	/// @brief 
	/// @param argc 
	/// @param argv 
	/// @return 
	bool Application::Init(int argc, char** argv)
	{
		JobSqueduler::CreateInstance();
		FrameSequencer::CreateInstance();

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
}
