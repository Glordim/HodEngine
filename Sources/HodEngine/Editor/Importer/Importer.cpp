#include "HodEngine/Editor/Importer/Importer.h"
#include "HodEngine/Editor/Project.h"
#include "HodEngine/Editor/Asset.h"

#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"
#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/UID.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Importer)
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

		core::FileStream metaFile(metaFilePath, core::FileMode::Read);
		if (metaFile.CanRead() == false)
		{
			metaFile.Close();
			if (GenerateNewMeta(metaFilePath) == false)
			{
				return false;
			}
			if (metaFile.Open(metaFilePath, core::FileMode::Read) == false || metaFile.CanRead() == false)
			{
				return false;
			}
		}
		
		core::Document document;
		core::DocumentReaderJson documentReader;
		if (documentReader.Read(document, metaFile) == false)
		{
			// TODO output reason
			return false;
		}

		const core::Document::Node* uidNode = document.GetRootNode().GetChild("uid");
		if (uidNode == nullptr)
		{
			// TODO output reason
			return false;
		}
		UID uid = UID::FromString(uidNode->GetString().c_str()); // TODO stringview for uid remove c_str

		const core::Document::Node* importerNode = document.GetRootNode().GetChild("importer");
		if (importerNode == nullptr)
		{
			// TODO output reason
			return false;
		}

		if (GetReflectionDescriptorV()->DeserializeFromDocument(*this, *importerNode) == false)
		{
			// TODO output reason
			return false;
		}

		core::FileStream dataFile(path, core::FileMode::Read);
		if (dataFile.CanRead() == false)
		{
			// TODO output reason
			return false;
		}

		Project* project = Project::GetInstance();

		std::filesystem::path resourceFilePath = project->GetResourceDirPath() / uid.ToString();
		resourceFilePath += ".resource";

		core::FileStream resourceFile(resourceFilePath, core::FileMode::Write);
		if (resourceFile.CanWrite() == false)
		{
			// TODO output reason
			return false;
		}

		return WriteResource(dataFile, metaFile, resourceFile);
	}

	/// @brief 
	/// @param metaFilePath 
	/// @return 
	bool Importer::GenerateNewMeta(const std::filesystem::path& metaFilePath)
	{
		core::FileStream metaFile(metaFilePath, core::FileMode::Write);
		if (metaFile.CanWrite() == true)
		{
			Meta meta;
			meta._uid = UID::GenerateUID();

			core::Document document;
			if (meta.GetReflectionDescriptorV()->SerializeInDocument(meta, document.GetRootNode()) == false)
			{
				return false;
			}

			if (GetReflectionDescriptorV()->SerializeInDocument(*this, document.GetRootNode().AddChild("importer")) == false)
			{
				return false;
			}

			core::DocumentWriterJson documentWriter;
			return documentWriter.Write(document, metaFile);
		}

		return true;
	}
}
