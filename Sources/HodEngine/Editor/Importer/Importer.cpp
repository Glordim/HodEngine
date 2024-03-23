#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <fstream>

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

		std::ifstream metaFile(metaFilePath);
		if (metaFile.is_open() == false)
		{
			metaFile.close();
			if (GenerateNewMeta(metaFilePath) == false)
			{
				return false;
			}
			metaFile.open(metaFilePath);
			if (metaFile.is_open() == false)
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

		std::ifstream dataFile(path);
		if (dataFile.is_open() == false)
		{
			// TODO output reason
			return false;
		}

		Project* project = Project::GetInstance();

		std::filesystem::path thumbnailFilePath = project->GetThumbnailDirPath() / meta._uid.ToString();
		thumbnailFilePath += ".png";

		std::ofstream thumbnailFile(thumbnailFilePath);
		if (thumbnailFile.is_open() == false)
		{
			// TODO output reason
			return false;
		}

		std::filesystem::path resourceFilePath = project->GetResourceDirPath() / meta._uid.ToString();
		resourceFilePath += ".dat";

		std::ofstream resourceFile(resourceFilePath);
		if (resourceFile.is_open() == false)
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
