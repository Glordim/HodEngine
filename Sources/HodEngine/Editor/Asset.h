#pragma once

#include <stdint.h>

#include <filesystem>

#include <HodEngine/Core/UID.h>
#include "HodEngine/Core/Reflection/ReflectionMacros.h"
#include "HodEngine/Editor/Importer/Importer.h"

namespace hod::editor
{
	// TODO embed in Asset when reflection support it
	struct Meta
	{
		REFLECTED_CLASS(Meta);

	public:

		bool			LoadImporterConfig(const core::Document::Node& documentNode);

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
		bool					Save();

		const UID&						GetUid() const;
		const std::filesystem::path&	GetPath() const;
		const std::string&				GetName() const;

		Meta&							GetMeta();

	private:

		Meta					_meta;

		std::string				_name;
		std::filesystem::path	_path;
	};
}
