#include "HodEngine/Editor/Asset.h"

#include "HodEngine/Editor/AssetDatabase.h"
#include "HodEngine/Editor/Editor.h"
#include "HodEngine/Editor/Project.h"

#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"

#include "HodEngine/Renderer/RHI/Texture.h"
#include "HodEngine/Renderer/Renderer.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Meta, void)
	{
		ADD_PROPERTY(Meta, _uid);
		ADD_PROPERTY(Meta, _importerType);
	}

	/// @brief 
	Asset::Asset(const std::filesystem::path& path)
		: _path(path)
	{
		_name = _path.stem().string();
		_meta._uid = UID::GenerateUID();

		_thumbnail = renderer::Renderer::GetInstance()->CreateTexture();

		_meta._importerSettings = AssetDatabase::GetInstance()->GetDefaultImporter().AllocateSettings();
	}

	/// @brief 
	Asset::~Asset()
	{
		delete _thumbnail;
	}

	/// @brief 
	/// @return 
	bool Asset::Load()
	{
		std::filesystem::path metaPath = _path;
		metaPath += ".meta";

		core::FileStream fileStream(metaPath, core::FileMode::Read);
		if (fileStream.CanRead() == false)
		{
			// TODO generate new meta if not exist
			AssetDatabase::GetInstance()->Import(_path);
			return false;
		}

		core::Document document;
		core::DocumentReaderJson documentReader;
		if (documentReader.Read(document, metaPath) == false)
		{
			return false;
		}

		if (Meta::GetReflectionDescriptor()->DeserializeFromDocument(_meta, document.GetRootNode()) == false)
		{
			return false;
		}
		if (_meta.LoadImporterConfig(document.GetRootNode()) == false) // TODO improve reflection
		{
			return false;
		}

		AssetDatabase::GetInstance()->ReimportAssetIfNecessary(shared_from_this());

		Project* project = Project::GetInstance();
		std::filesystem::path thumbnailFilePath = project->GetThumbnailDirPath() / _meta._uid.ToString();
		thumbnailFilePath += ".png";

		_thumbnail->LoadFromPath(thumbnailFilePath.string().c_str());

		return true;
	}

	/// @brief 
	/// @return 
	bool Asset::Save(Object* object)
	{
		core::Document metaDocument;
		if (Meta::GetReflectionDescriptor()->SerializeInDocument(_meta, metaDocument.GetRootNode()) == false)
		{
			return false;
		}
		if (_meta.SaveImporterConfig(metaDocument.GetRootNode()) == false) // TODO improve reflection
		{
			return false;
		}

		std::filesystem::path metaPath = _path;
		metaPath += ".meta";

		core::DocumentWriterJson documentWriter;
		if (documentWriter.Write(metaDocument, metaPath) == false)
		{
			return false;
		}

		if (object != nullptr)
		{
			core::Document objectDocument;
			if (object->GetReflectionDescriptorV()->SerializeInDocument((const void*)object, objectDocument.GetRootNode()) == false)
			{
				return false;
			}

			if (documentWriter.Write(objectDocument, _path) == false)
			{
				return false;
			}
		}

		_dirty = false;
		return true;
	}

	/// @brief 
	/// @return 
	const UID& Asset::GetUid() const
	{
		return _meta._uid;
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Asset::GetPath() const
	{
		return _path;
	}

	/// @brief 
	/// @return 
	const std::string& Asset::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @return 
	bool Asset::IsDirty() const
	{
		return _dirty;
	}
	
	/// @brief 
	void Asset::SetDirty()
	{
		_dirty = true;
	}

	/// @brief 
	/// @return 
	Meta& Asset::GetMeta()
	{
		return _meta;
	}

	/// @brief 
	/// @return 
	renderer::Texture* Asset::GetThumbnail() const
	{
		return _thumbnail;
	}

	/// @brief 
	/// @param documentNode 
	/// @return 
	bool Meta::LoadImporterConfig(const core::Document::Node& documentNode)
	{
		Importer* importer = AssetDatabase::GetInstance()->GetImporter(_importerType);
		if (importer == nullptr)
		{
			// TODO message;
			return false;
		}

		// TODO Ensure _importerSettings == nullptr

		_importerSettings = importer->AllocateSettings();
		if (_importerSettings == nullptr)
		{
			// TODO message;
			return false;
		}

		if (_importerSettings->GetReflectionDescriptorV()->DeserializeFromDocument((void*)_importerSettings, documentNode) == false)
		{
			// TODO message;
			return false;
		}
		return true;
	}

	/// @brief 
	/// @param documentNode 
	/// @return 
	bool Meta::SaveImporterConfig(core::Document::Node& documentNode) const
	{
		// TODO Ensure _importerSettings == nullptr

		if (_importerSettings->GetReflectionDescriptorV()->SerializeInDocument((const void*)_importerSettings, documentNode) == false)
		{
			// TODO message;
			return false;
		}
		return true;
	}
}
