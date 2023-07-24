#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/UID.h>
#include "HodEngine/Core/Reflection/ReflectionMacros.h"
#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::renderer
{
	class Texture;
}

namespace hod::editor
{
	// TODO embed in Asset when reflection support it
	struct Meta
	{
		REFLECTED_CLASS_NO_PARENT(Meta)

	public:

		bool			LoadImporterConfig(const Document::Node& documentNode);
		bool			SaveImporterConfig(Document::Node& documentNode) const;

	public:

		UID						_uid;
		std::string				_importerType;
		ImporterSettings*		_importerSettings = nullptr;
	};

	/// @brief 
	class Asset : public std::enable_shared_from_this<Asset>
	{
	public:
								Asset(const std::filesystem::path& path);
								~Asset();

		bool					Load();
		bool					Save(void* instance, ReflectionDescriptor* reflectionDescriptor);
		bool					Save(Object* object = nullptr);

		const UID&						GetUid() const;
		const std::filesystem::path&	GetPath() const;
		const std::string&				GetName() const;

		Meta&							GetMeta();

		renderer::Texture*				GetThumbnail() const;

		bool							IsDirty() const;
		void							SetDirty();

	private:

		bool					_dirty = false;

		Meta					_meta;

		std::string				_name;
		std::filesystem::path	_path;

		renderer::Texture*		_thumbnail;
	};
}
