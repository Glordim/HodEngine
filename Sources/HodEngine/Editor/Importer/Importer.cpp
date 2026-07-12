#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/ScopedGuard.hpp"
#include "HodEngine/Core/Stream/SpillStream.hpp"
#include "HodEngine/Core/Time/SystemTime.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/UID.hpp"

#include <cstdint>

#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/Hash.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(ImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	void Importer::SetAssetType(std::string_view assetType)
	{
		_assetType = Hash::ComputeXxh3_64(assetType);
	}

	bool Importer::Import(const Path& sourcePath, const Path& destinationPath, const UID& uid, ImporterSettings* importSettings, uint64_t taskId)
	{
		_taskId = taskId;
		_sourcePath = sourcePath;
		_additionalSources.Clear();
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 0.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Opening source");

		FileStream sourceStream;
		if (sourceStream.Open(sourcePath) == false)
		{
			OUTPUT_ERROR("Importer::Import: Unable to open source file '{}'", sourcePath);
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 2.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Creating tmp folder");
		_tmpDir = FileSystem::GetTemporaryPath() / "HodEngine" / Project::GetInstance()->GetName() / "Import" / uid.ToString(); // todo add datetime
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
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Processing source");
		if (FillDataBlock(sourceStream, importSettings) == false)
		{
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}
		sourceStream.Seek(0, Stream::SeekOrigin::Begin);

		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Computing source hash");
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 80.0f);

		char buffer[256 * 1024];
		void* hashState = nullptr;
		uint64_t fileHash = 0;
		int32_t readBytes = 0;
		uint32_t sourceSize = sourceStream.GetSize();
		while (true)
		{
			readBytes = sourceStream.Read(buffer, sizeof(buffer));
			if (readBytes > 0)
			{
				Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 80.0f + 5.0f * (static_cast<float>(readBytes) / static_cast<float>(sourceSize)));
				hashState = Hash::ComputeXxh3_64_Cumulated(hashState, buffer, readBytes);
				if (readBytes != sizeof(buffer))
				{
					fileHash = Hash::ComputeXxh3_64_Result(hashState);
					break;
				}
			}
			else
			{
				OUTPUT_ERROR("Importer::Import: Unable to read source file '{}'", sourcePath);
				Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
				return false;
			}
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Writing asset");
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 85.0f);

		AssetContainer assetContainer;
		assetContainer.SetUid(uid);
		assetContainer.SetAssetType(_assetType);
		assetContainer.AddSource(sourcePath, fileHash);
		for (const AssetContainer::SourceInfo& additionalSource : _additionalSources)
		{
			assetContainer.AddSource(additionalSource._path, additionalSource._hash);
		}

		if (importSettings != nullptr)
		{
			Serializer::Serialize(*importSettings, assetContainer.GetImportSettings());
		}

		for (DataBlock* dataBlock : _dataBlocks)
		{
			dataBlock->GetStream().Flush();
			assetContainer.SetDataBlock(dataBlock->GetName(), dataBlock->GetStream(), dataBlock->GetCompressed());
		}

		if (assetContainer.Save(destinationPath, _tmpDir))
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
	}

	void Importer::AddSource(const Path& sourcePath, uint64_t sourceHash)
	{
		AssetContainer::SourceInfo& source = _additionalSources.EmplaceBack();
		source._path = sourcePath;
		source._hash = sourceHash;
	}

	Stream& Importer::AddDataBlockStream(std::string_view name, bool compressed)
	{
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

	void Importer::UpdateFillDataBlockProgress(float percent)
	{
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 5.0f + 75.0f * percent);
	}

	void Importer::UpdateFillDataBlockDescription(std::string_view description)
	{
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, description);
	}
}
