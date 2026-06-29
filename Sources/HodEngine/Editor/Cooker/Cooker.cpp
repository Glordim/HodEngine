#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/ScopedGuard.hpp"
#include "HodEngine/Core/UID.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"

#include "HodEngine/Core/Hash.hpp"

namespace hod::inline editor
{
	void Cooker::SetAssetType(std::string_view assetType)
	{
		_assetType = Hash::ComputeXxh3_64(assetType.data(), assetType.size());
	}

	bool Cooker::Cook(const Path& path, uint8_t platforms, uint8_t configs, uint32_t languages, uint64_t taskId)
	{
		AssetDatabase::FileSystemMapping* fileSystemNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(path);
		if (fileSystemNode != nullptr && fileSystemNode->_asset != nullptr)
		{
			return Cook(*fileSystemNode->_asset, platforms, configs, languages, taskId);
		}
		else
		{
			OUTPUT_ERROR("Cooker::Cook: Unable to retrieve asset at path {}", path);
			return false;
		}
	}
	
	bool Cooker::Cook(const Asset& asset, uint8_t platforms, uint8_t configs, uint32_t languages, uint64_t taskId)
	{
		_taskId = taskId;
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 0.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Opening asset");

		FileStream assetStream;
		if (assetStream.Open(asset.GetPath()) == false)
		{
			OUTPUT_ERROR("Cooker::Cook: Unable to open asset file '{}'", asset.GetPath());
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 2.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Creating tmp folder");
		_tmpDir = FileSystem::GetTemporaryPath() / "HodEngine" / Project::GetInstance()->GetName() / "Cook" / asset.GetUid().ToString(); // todo add datetime
		ScopedGuard cleanupTmp = [&]()
		{
			FileSystem::GetInstance()->RemoveAll(_tmpDir);
		};
		if (FileSystem::GetInstance()->CreateDirectories(_tmpDir) == false)
		{
			OUTPUT_ERROR("Importer::Import: Unable to create tmp dir '{}'", _tmpDir);
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 5.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Processing asset");
		if (FillDataBlock(asset, platforms, configs, languages) == false)
		{
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}
		assetStream.Seek(0, Stream::SeekOrigin::Begin);

		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Writing resource");
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 80.0f);
		/*
		ResourceContainer resourceContainer;
		resourceContainer.SetType();
		resourceContainer.SetUid(uid);
		resourceContainer.SetAssetContentHash(assetContentHashPath);

		for (DataBlock* dataBlock : _dataBlocks)
		{
			resourceContainer.SetDataBlock(dataBlock->GetName(), dataBlock->GetStream(), dataBlock->GetCompressed());
		}

		Path destinationPath = Project::GetInstance()->GetCacheBuildDirPath() / "Resource" / asset.GetUid().ToString() + ".res"
		if (resourceContainer.Save(destinationPath, _tmpDir))
		{
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Succeeded);
			Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 100.0f);
			return true;
		}
		else
		{
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}
		*/
		return true;
	}

	Stream& Cooker::AddDataBlockStream(std::string_view name, bool compressed, uint8_t platforms, uint8_t configs, uint32_t languages)
	{
		(void)platforms;
		(void)configs;
		(void)languages;

		for (DataBlock* dataBlock : _dataBlocks)
		{
			if (dataBlock->GetName() == name)
			{
				return dataBlock->GetStream();
			}
		}
		_dataBlocks.PushBack(DefaultAllocator::GetInstance().New<DataBlock>(name, _tmpDir / name, compressed));
		return _dataBlocks.Back()->GetStream();
	}

	void Cooker::UpdateFillDataBlockProgress(float percent)
	{
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 5.0f + 75.0f * percent);
	}

	void Cooker::UpdateFillDataBlockDescription(std::string_view description)
	{
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, description);
	}
}
