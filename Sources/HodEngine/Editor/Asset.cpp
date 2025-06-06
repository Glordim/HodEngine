#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include <fstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Meta, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Meta::_uid, "_uid");
		AddPropertyT(reflectionDescriptor, &Meta::_importerType, "_importerType");
	}

	/// @brief 
	Asset::Asset(const std::filesystem::path& path)
		: _path(path)
	{
		_name = _path.stem().string();
		_meta._uid = UID::GenerateUID();

		_meta._importerSettings = AssetDatabase::GetInstance()->GetDefaultImporter().AllocateSettings();
	}

	/// @brief 
	Asset::~Asset()
	{
		DefaultAllocator::GetInstance().Delete(_thumbnail);
	}

	/// @brief 
	/// @param path 
	void Asset::SetPath(const std::filesystem::path& path)
	{
		_path = path;
		_name = path.stem().string();
	}

	/// @brief 
	/// @param instance 
	/// @param reflectionDescriptor 
	void Asset::SetInstanceToSave(const void* instance, ReflectionDescriptor* reflectionDescriptor)
	{
		_instanceToSave = instance;
		_instanceToSaveReflectionDescriptor = reflectionDescriptor;
	}

	/// @brief 
	/// @return 
	bool Asset::Load()
	{
		std::filesystem::path metaPath = _path;
		metaPath += ".meta";

		if (std::filesystem::exists(metaPath) == false)
		{
			// TODO generate new meta if not exist
			if (AssetDatabase::GetInstance()->Import(_path) == false)
			{
				return false;
			}
			if (std::filesystem::exists(metaPath) == false)
			{
				return false;
			}
		}

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, metaPath) == false)
		{
			return false;
		}

		if (Serializer::Deserialize(_meta, document.GetRootNode()) == false)
		{
			return false;
		}
		const Document::Node* importerSettingsNode = document.GetRootNode().GetChild("importerSettings");
		if (importerSettingsNode != nullptr && _meta.LoadImporterConfig(*importerSettingsNode) == false) // TODO improve reflection
		{
			return false;
		}

		AssetDatabase::GetInstance()->ReimportAssetIfNecessary(shared_from_this());

		Project* project = Project::GetInstance();
		std::filesystem::path thumbnailFilePath = project->GetThumbnailDirPath() / _meta._uid.ToString();
		thumbnailFilePath += ".png";

		if (_thumbnail != nullptr)
		{
			DefaultAllocator::GetInstance().Delete(_thumbnail);
			_thumbnail = nullptr;
		}
		
		_thumbnail = renderer::Renderer::GetInstance()->CreateTexture();
		if (_thumbnail->LoadFromPath(thumbnailFilePath.string().c_str()) == false)
		{
			DefaultAllocator::GetInstance().Delete(_thumbnail);
			_thumbnail = nullptr;
		}

		return true;
	}

	/// @brief 
	/// @return 
	bool Asset::Save()
	{
		return Save(_instanceToSave, _instanceToSaveReflectionDescriptor);
	}

	/// @brief 
	/// @param instance 
	/// @param reflectionDescriptor 
	/// @return 
	bool Asset::Save(const void* instance, ReflectionDescriptor* reflectionDescriptor)
	{
		Document metaDocument;
		if (Serializer::Serialize(_meta, metaDocument.GetRootNode()) == false)
		{
			return false;
		}
		if (_meta.SaveImporterConfig(metaDocument.GetRootNode().AddChild("importerSettings")) == false) // TODO improve reflection
		{
			return false;
		}

		std::filesystem::path metaPath = _path;
		metaPath += ".meta";

		DocumentWriterJson documentWriter;
		if (documentWriter.Write(metaDocument, metaPath) == false)
		{
			return false;
		}

		if (instance != nullptr)
		{
			Document objectDocument;
			if (Serializer::Serialize(*reflectionDescriptor, instance, objectDocument.GetRootNode()) == false)
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
	const String& Asset::GetName() const
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
	void Asset::ResetDirty()
	{
		_dirty = false;
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
	/// @param importerSettings 
	/// @param importerType 
	void Meta::SetImporterConfig(std::shared_ptr<ImporterSettings> importerSettings, const char* importerType)
	{
		_importerSettings = importerSettings;
		_importerType = importerType;
	}

	/// @brief 
	/// @param documentNode 
	/// @return 
	bool Meta::LoadImporterConfig(const Document::Node& documentNode)
	{
		Importer* importer = AssetDatabase::GetInstance()->GetImporter(_importerType);
		if (importer != nullptr)
		{
			_importerSettings = importer->AllocateSettings();
		}

		if (_importerSettings == nullptr)
		{
			// TODO message;
			return false;
		}

		if (Serializer::Deserialize(*_importerSettings.get(), documentNode) == false)
		{
			// TODO message;
			return false;
		}
		return true;
	}

	/// @brief 
	/// @param documentNode 
	/// @return 
	bool Meta::SaveImporterConfig(Document::Node& documentNode) const
	{
		// TODO Ensure _importerSettings == nullptr

		if (Serializer::Serialize(_importerSettings.get(), documentNode) == false)
		{
			// TODO message;
			return false;
		}
		return true;
	}
}
