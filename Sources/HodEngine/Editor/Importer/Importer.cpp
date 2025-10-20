#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Editor/Project.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Resource/Resource.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/UID.hpp"

#include <fstream>
#include <sstream>

namespace hod::editor
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
		if (documentReader.Read(metaDocument, metaFileHandle) == false)
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

		const Document::Node* importerNode = metaDocument.GetRootNode().GetChild("importerSettings");
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

		std::ofstream thumbnailFile(thumbnailFilePath.GetString().CStr(), std::ios::binary);
		if (thumbnailFile.is_open() == false)
		{
			// TODO output reason
			FileSystem::GetInstance()->Close(metaFileHandle);
			FileSystem::GetInstance()->Close(dataFile);
			return false;
		}

		FileSystem::GetInstance()->Seek(metaFileHandle, 0, FileSystem::SeekMode::Begin);

		Document               document;
		Vector<Resource::Data> datas;
		bool                   result = WriteResource(dataFile, metaFileHandle, document, datas, thumbnailFile, *meta._importerSettings);

		Path resourceFilePath = project->GetResourceDirPath() / meta._uid.ToString().CStr();
		resourceFilePath += ".dat";

		std::ofstream resourceFile(resourceFilePath.GetString().CStr(), std::ios::binary);
		if (resourceFile.is_open() == false)
		{
			// TODO output reason
			return false;
		}

		resourceFile.write("HodResource", 11);

		uint32_t version = 1;
		resourceFile.write(reinterpret_cast<char*>(&version), sizeof(version));

		std::stringstream documentStringStream;

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, documentStringStream) == false)
		{
			// TODO message
			return false;
		}

		uint32_t documentLen = (uint32_t)documentStringStream.str().size();
		resourceFile.write(reinterpret_cast<char*>(&documentLen), sizeof(documentLen));

		// todo use documentStringStream ?
		if (documentWriter.Write(document, resourceFile) == false)
		{
			// TODO message
			return false;
		}

		uint32_t dataCount = (uint32_t)datas.size();
		resourceFile.write(reinterpret_cast<char*>(&dataCount), sizeof(dataCount));

		for (const Resource::Data& data : datas)
		{
			resourceFile.write(reinterpret_cast<const char*>(&data._size), sizeof(data._size));
			resourceFile.write(static_cast<const char*>(data._buffer), data._size);

			DefaultAllocator::GetInstance().Free(data._buffer);
		}
		FileSystem::GetInstance()->Close(metaFileHandle);
		FileSystem::GetInstance()->Close(dataFile);
		return result;
	}

	/// @brief
	/// @param metaFilePath
	/// @return
	bool Importer::GenerateNewMeta(const Path& metaFilePath)
	{
		std::ofstream metaFile(metaFilePath.GetString().CStr());
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
