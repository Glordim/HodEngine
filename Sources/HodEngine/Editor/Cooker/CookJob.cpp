#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Cooker/CookJob.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"
#include <cstdint>

namespace hod::inline editor
{
	CookJob::CookJob(const Path& asset, uint8_t platforms, uint8_t configs, uint32_t languages)
	: _assetPath(asset)
	, _platforms(platforms)
	, _configs(configs)
	, _languages(languages)
	{
		AssetDatabase::FileSystemMapping* filesystemNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(asset);
		if (filesystemNode != nullptr && filesystemNode->_asset != nullptr)
		{
			_asset = filesystemNode->_asset.get();
		}

		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();
		_taskId = taskTracker.CreateTask(String::Format("Cook %s", _assetPath.Filename().CStr()).CStr(), _assetPath.CStr());
	}

	CookJob::CookJob(const Asset& asset, uint8_t platforms, uint8_t configs, uint32_t languages)
	: _asset(&asset)
	, _platforms(platforms)
	, _configs(configs)
	, _languages(languages)
	{
		_assetPath = _asset->GetPath();

		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();
		_taskId = taskTracker.CreateTask(String::Format("Cook %s", _assetPath.Filename().CStr()).CStr(), _assetPath.CStr());
	}

	void CookJob::Execution()
	{
		TaskTracker& taskTracker = Editor::GetInstance()->GetTaskTracker();
		taskTracker.UpdateTaskProgress(_taskId, -1.0f);

		if (_asset != nullptr)
		{
			uint64_t type = _asset->GetType();
			Cooker* cooker = AssetDatabase::GetInstance()->AcquireCooker(type);
			if (cooker != nullptr)
			{
				cooker->Cook(*_asset, _platforms, _configs, _languages, _taskId);
				AssetDatabase::GetInstance()->ReleaseCooker(cooker);
			}
			else
			{
				OUTPUT_ERROR("Cook fail, any compatible Cooker for '{}' asset", _asset->GetPath());
				taskTracker.UpdateTaskDescription(_taskId, "Cooker not found");
				taskTracker.UpdateTaskStatus(_taskId, TaskStatus::Failed);
			}
		}
		else
		{
			OUTPUT_ERROR("CookJob::Execution: asset {} not found", _assetPath);
			taskTracker.UpdateTaskDescription(_taskId, "Hasn't extension");
			taskTracker.UpdateTaskStatus(_taskId, TaskStatus::Failed);
		}
	}
}
