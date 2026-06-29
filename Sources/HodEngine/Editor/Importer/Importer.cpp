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
#include "HodEngine/GameSystems/Resource/Resource.hpp"
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

	/// @brief
	/// @param path
	/// @return
	bool Importer::CanImport(const Path& path)
	{
		String extension = path.Extension().GetString().SubStr(1);
		for (char& c : extension)
		{
			c = static_cast<char>(std::tolower((int)c));
		}

		for (const char* supportedExtension : _supportedDataFileExtensions)
		{
			if (extension == supportedExtension)
			{
				return true;
			}
		}
		return false;
	}

	bool Importer::CheckSupportedExtensions(std::string_view extension) const
	{
		String extensionLower(extension);
		for (char& c : extensionLower)
		{
			c = static_cast<char>(std::tolower((int)c));
		}
		for (const char* supportedExtension : _supportedDataFileExtensions)
		{
			if (extensionLower == supportedExtension)
			{
				return true;
			}
		}
		return false;
	}

	void Importer::SetAssetType(std::string_view assetType)
	{
		_assetType = Hash::ComputeXxh3_64(assetType.data(), assetType.size());
	}

	bool Importer::Import(const Path& sourcePath, const Path& destinationPath, const UID& uid, ImporterSettings* importSettings, uint64_t taskId)
	{
		_taskId = taskId;
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
		assetContainer.SetSourcePath(sourcePath);
		assetContainer.SetSourceHash(fileHash);

		if (importSettings != nullptr)
		{
			Serializer::Serialize(*importSettings, assetContainer.GetImportSettings());
		}

		for (DataBlock* dataBlock : _dataBlocks)
		{
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

	/// @brief
	/// @param path
	/// @return
	bool Importer::Import(const Path& path)
	{
		Path metaFilePath = path;
		metaFilePath += ".meta";

		FileSystem::Handle metaFileHandle = FileSystem::GetInstance()->Open(metaFilePath);
		if (metaFileHandle.IsOpen() == false)
		{
			FileSystem::GetInstance()->Close(metaFileHandle);
			if (GenerateNewMeta(metaFilePath) == false)
			{
				return false;
			}
			metaFileHandle = FileSystem::GetInstance()->Open(metaFilePath);
			if (metaFileHandle.IsOpen() == false)
			{
				return false;
			}
		}

		Document           metaDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(metaDocument, metaFilePath) == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		Meta meta;
		if (Serializer::Deserialize(meta, metaDocument.GetRootNode()) == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		const DocumentNode* importerNode = metaDocument.GetRootNode().GetChild("importerSettings");
		if (importerNode == nullptr)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		meta._importerSettings = AllocateSettings();
		if (Serializer::Deserialize(*meta._importerSettings.get(), *importerNode) == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		FileSystem::Handle dataFile = FileSystem::GetInstance()->Open(path); //, std::ios::binary);
		if (dataFile.IsOpen() == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		Project* project = Project::GetInstance();

		Path thumbnailFilePath = project->GetThumbnailDirPath() / meta._uid.ToString().CStr();
		thumbnailFilePath += ".png";

		FileStream thumbnailFile;
		if (thumbnailFile.Open(thumbnailFilePath, FileSystem::OpenMode::Write) == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			FileSystem::GetInstance()->Close(dataFile);
			return false;
		}

		FileSystem::GetInstance()->Seek(metaFileHandle, 0, FileSystem::SeekMode::Begin);

		FileSystem::GetInstance()->Close(metaFileHandle);
		FileSystem::GetInstance()->Close(dataFile);

		FileStream dataFileStream;
		dataFileStream.Open(path);

		FileStream metaFileStream;
		metaFileStream.Open(metaFilePath);

		Document               document;
		Vector<Resource::Data> datas;
		bool                   result = WriteResource(dataFileStream, metaFileStream, document, datas, thumbnailFile, *meta._importerSettings);

		Path resourceFilePath = project->GetResourceDirPath() / meta._uid.ToString().CStr();
		resourceFilePath += ".dat";

		FileStream resourceFile;
		if (resourceFile.Open(resourceFilePath, FileSystem::OpenMode::Write) == false)
		{
			// TODO output reason
			return false;
		}

		resourceFile.Write("HodResource", 11);

		uint32_t version = 1;
		resourceFile.Write(&version, sizeof(version));

		String documentString;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentString) == false)
		{
			// TODO message
			return false;
		}

		uint32_t documentLen = documentString.Length();
		resourceFile.Write(&documentLen, sizeof(documentLen));
		resourceFile.Write(documentString.CStr(), documentString.Length());

		uint32_t dataCount = (uint32_t)datas.Size();
		resourceFile.Write(&dataCount, sizeof(dataCount));

		for (const Resource::Data& data : datas)
		{
			resourceFile.Write(&data._size, sizeof(data._size));
			resourceFile.Write(data._buffer, data._size);

			DefaultAllocator::GetInstance().Free(data._buffer);
		}
		return result;
	}

	/// @brief
	/// @param metaFilePath
	/// @return
	bool Importer::GenerateNewMeta(const Path& metaFilePath)
	{
		Meta meta;
		meta._uid = UID::GenerateUID();
		meta._importerType = GetTypeName();

		Document document;
		if (Serializer::Serialize(meta, document.GetRootNode()) == false)
		{
			return false;
		}

		std::shared_ptr<ImporterSettings> settings = AllocateSettings();
		if (Serializer::Serialize(settings.get(), document.GetRootNode().AddChild("importerSettings")) == false)
		{
			return false;
		}

		DocumentWriterJson documentWriter;
		return documentWriter.Write(document, metaFilePath);
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
