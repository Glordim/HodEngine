#include "Editor.h"

#include "HodEngine/Core/Src/Job/Job.h"

namespace HOD
{
	/// @brief 
	Editor::Editor()
		: _updateJob(this, &Editor::Update, JobQueue::Queue::FramedNormalPriority)
	{
		
	}

	/// @brief 
	void Editor::Update()
	{
		ImGui::
	}
}
