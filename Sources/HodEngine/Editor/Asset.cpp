#include "HodEngine/Editor/Asset.h"

#include "HodEngine/Editor/AssetDatabase.h"
#include "HodEngine/Editor/Editor.h"

#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Meta)
	{
		core::Reflection::Property::Object<UID>* uid = new core::Reflection::Property::Object<UID>(offsetof(Meta, _uid), "uid");
		AddProperty(uid);
	}

	/// @brief 
	Asset::Asset(const std::filesystem::path& path)
		: _path(path)
	{
		_name = _path.stem().string();
		_meta._uid = UID::GenerateUID();
	}

	/// @brief 
	Asset::~Asset()
	{
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

		AssetDatabase::GetInstance()->ReimportAssetIfNecessary(shared_from_this());

		return true;
	}

	/// @brief 
	/// @return 
	bool Asset::Save()
	{
		core::Document document;
		if (Meta::GetReflectionDescriptor()->SerializeInDocument(_meta, document.GetRootNode()) == false)
		{
			return false;
		}

		std::filesystem::path metaPath = _path;
		metaPath += ".meta";

		core::DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, metaPath) == false)
		{
			return false;
		}
		
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
}
