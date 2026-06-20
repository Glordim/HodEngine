#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/ImportJob.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"
#include <cstdint>

namespace hod::inline editor
{
	ImportJob::ImportJob(const Path& sourceFilePath, const Path& destinationDirPath)
	: _sourceFilePath(sourceFilePath)
	, _destinationDirPath(destinationDirPath)
	{

	}

	void ImportJob::Execution()
	{
		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();
		uint64_t taskId = taskTracker.CreateTask(String::Format("Import %s", _sourceFilePath.Filename().CStr()).CStr(), _sourceFilePath.CStr());
		taskTracker.UpdateTaskProgress(taskId, -1.0f);

		
	}
}
