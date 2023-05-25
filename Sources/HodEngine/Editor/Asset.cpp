#include "HodEngine/Editor/Asset.h"

#include "HodEngine/Editor/AssetDatabase.h"
#include "HodEngine/Editor/Editor.h"

#include "HodEngine/Core/Stream/FileStream.h"
#include "HodEngine/Core/Document/DocumentReaderJson.h"
#include "HodEngine/Core/Document/DocumentWriterJson.h"

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(Meta)
	{
		core::Reflection::Property::Variable* uid = new core::Reflection::Property::Variable(core::Reflection::Property::Variable::UInt64, offsetof(Meta, _uid), "UID");
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
		std::filesystem::path metaPath = _path / ".meta";

		core::FileStream fileStream(metaPath, core::FileMode::Read);
		if (fileStream.CanRead() == true)
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

		std::filesystem::path metaPath = _path / ".meta";

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
}
