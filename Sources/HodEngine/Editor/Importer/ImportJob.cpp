#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/ImportJob.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"
#include <cstdint>

namespace hod::inline editor
{
	ImportJob::ImportJob(const Path& sourceFilePath, const Path& destinationDirPath)
	: _sourceFilePath(sourceFilePath)
	, _destinationDirPath(destinationDirPath)
	{
		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();
		_taskId = taskTracker.CreateTask(String::Format("Import %s", _sourceFilePath.Filename().CStr()).CStr(), _sourceFilePath.CStr());
	}

	void ImportJob::Execution()
	{
		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();
		taskTracker.UpdateTaskProgress(_taskId, -1.0f);

		if (_sourceFilePath.HasExtension())
		{
			String extension = _sourceFilePath.Extension().GetString().SubStr(1);
			Importer* importer = AssetDatabase::GetInstance()->AcquireImporter(extension);
			if (importer != nullptr)
			{
				Path destinationFilePath = _destinationDirPath / _sourceFilePath.Filename().ReplaceExtension(importer->GetAssetExtension());
				importer->Import(_sourceFilePath, destinationFilePath, UID::GenerateUID(), nullptr, _taskId);
				AssetDatabase::GetInstance()->ReleaseImporter(importer);
			}
			else
			{
				OUTPUT_ERROR("Import '{}' fail, any Importer register for '{}' extension", _sourceFilePath, extension);
				taskTracker.UpdateTaskDescription(_taskId, "Hasn't extension");
				taskTracker.UpdateTaskStatus(_taskId, TaskStatus::Failed);
			}
		}
		else
		{
			OUTPUT_ERROR("Import '{}' fail, hasn't extension", _sourceFilePath);
			taskTracker.UpdateTaskDescription(_taskId, "Hasn't extension");
			taskTracker.UpdateTaskStatus(_taskId, TaskStatus::Failed);
		}
	}
}
