#include "HodEngine/Editor/Importer/Importer.h"
#include "HodEngine/Editor/Project.h"
#include "HodEngine/Editor/Asset.h"

#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/UID.h"
#include "HodEngine/Core/Serialization/Serializer.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(ImporterSettings, Object)
	{
		
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool Importer::CanImport(const std::filesystem::path& path)
	{
		std::string extension = path.extension().string().substr(1);
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

		FileStream metaFile(metaFilePath, FileMode::Read);
		if (metaFile.CanRead() == false)
		{
			metaFile.Close();
			if (GenerateNewMeta(metaFilePath) == false)
			{
				return false;
			}
			if (metaFile.Open(metaFilePath, FileMode::Read) == false || metaFile.CanRead() == false)
			{
				return false;
			}
		}
		
		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, metaFile) == false)
		{
			// TODO output reason
			return false;
		}

		Meta meta;
		if (Serializer::Deserialize(meta, document.GetRootNode()) == false)
		{
			// TODO output reason
			return false;
		}

		const Document::Node* importerNode = document.GetRootNode().GetChild("importerSettings");
		if (importerNode == nullptr)
		{
			// TODO output reason
			return false;
		}

		meta._importerSettings = AllocateSettings();
		if (Serializer::Deserialize(meta._importerSettings, *importerNode) == false)
		{
			// TODO output reason
			return false;
		}

		FileStream dataFile(path, FileMode::Read);
		if (dataFile.CanRead() == false)
		{
			// TODO output reason
			return false;
		}

		Project* project = Project::GetInstance();

		std::filesystem::path thumbnailFilePath = project->GetThumbnailDirPath() / meta._uid.ToString();
		thumbnailFilePath += ".png";

		FileStream thumbnailFile(thumbnailFilePath, FileMode::Write);
		if (thumbnailFile.CanWrite() == false)
		{
			// TODO output reason
			return false;
		}

		std::filesystem::path resourceFilePath = project->GetResourceDirPath() / meta._uid.ToString();
		resourceFilePath += ".resource";

		FileStream resourceFile(resourceFilePath, FileMode::Write);
		if (resourceFile.CanWrite() == false)
		{
			// TODO output reason
			return false;
		}

		return WriteResource(dataFile, metaFile, resourceFile, thumbnailFile, *meta._importerSettings);
	}

	/// @brief 
	/// @param metaFilePath 
	/// @return 
	bool Importer::GenerateNewMeta(const std::filesystem::path& metaFilePath)
	{
		FileStream metaFile(metaFilePath, FileMode::Write);
		if (metaFile.CanWrite() == true)
		{
			Meta meta;
			meta._uid = UID::GenerateUID();
			meta._importerType = GetTypeName();

			Document document;
			if (Serializer::Serialize(meta, document.GetRootNode()) == false)
			{
				return false;
			}

			ImporterSettings* settings = AllocateSettings();
			if (Serializer::Serialize(settings, document.GetRootNode().AddChild("importerSettings")) == false)
			{
				return false;
			}
			delete settings;

			DocumentWriterJson documentWriter;
			return documentWriter.Write(document, metaFile);
		}

		return true;
	}
}
