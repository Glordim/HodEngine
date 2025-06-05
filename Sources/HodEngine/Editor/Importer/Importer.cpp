#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <fstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(ImporterSettings, reflectionDescriptor)
	{
		
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Importer::CanImport(const std::filesystem::path& path)
	{
		String extension = path.extension().string().substr(1);
		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });

		for (const char* supportedExtension : _supportedDataFileExtensions)
		{
			if (extension == supportedExtension)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Importer::Import(const std::filesystem::path& path)
	{
		std::filesystem::path metaFilePath = path;
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
		
		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, metaFileHandle) == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		Meta meta;
		if (Serializer::Deserialize(meta, document.GetRootNode()) == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			return false;
		}

		const Document::Node* importerNode = document.GetRootNode().GetChild("importerSettings");
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

		std::filesystem::path thumbnailFilePath = project->GetThumbnailDirPath() / meta._uid.ToString();
		thumbnailFilePath += ".png";

		std::ofstream thumbnailFile(thumbnailFilePath, std::ios::binary);
		if (thumbnailFile.is_open() == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			FileSystem::GetInstance()->Close(dataFile);
			return false;
		}

		std::filesystem::path resourceFilePath = project->GetResourceDirPath() / meta._uid.ToString();
		resourceFilePath += ".dat";

		std::ofstream resourceFile(resourceFilePath, std::ios::binary);
		if (resourceFile.is_open() == false)
		{
			// TODO output reason
			return false;
		}

		resourceFile.write("HodResource", 11);
		bool result = WriteResource(dataFile, metaFileHandle, resourceFile, thumbnailFile, *meta._importerSettings);
		FileSystem::GetInstance()->Close(metaFileHandle);
		FileSystem::GetInstance()->Close(dataFile);
		return result;
	}

	/// @brief 
	/// @param metaFilePath 
	/// @return 
	bool Importer::GenerateNewMeta(const std::filesystem::path& metaFilePath)
	{
		std::ofstream metaFile(metaFilePath);
		if (metaFile.is_open() == true)
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
			return documentWriter.Write(document, metaFile);
		}

		return true;
	}
}
