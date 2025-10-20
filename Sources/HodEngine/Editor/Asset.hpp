#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::renderer
{
	class Texture;
}

namespace hod::editor
{
	// TODO embed in Asset when reflection support it
	struct HOD_EDITOR_API Meta
	{
		REFLECTED_CLASS_NO_VIRTUAL(Meta)

	public:
		bool LoadImporterConfig(const Document::Node& documentNode);
		bool SaveImporterConfig(Document::Node& documentNode) const;

		void SetImporterConfig(std::shared_ptr<ImporterSettings> importerSettings, const char* importerType);

	public:
		UID                               _uid;
		String                            _importerType;
		std::shared_ptr<ImporterSettings> _importerSettings;
	};

	/// @brief
	class HOD_EDITOR_API Asset : public std::enable_shared_from_this<Asset>
	{
	public:
		Asset(const Path& path);
		~Asset();

		bool Load();
		bool Save(const void* instance, ReflectionDescriptor* reflectionDescriptor);
		bool Save();

		const UID&    GetUid() const;
		const Path&   GetPath() const;
		const String& GetName() const;

		Meta& GetMeta();

		renderer::Texture* GetThumbnail() const;

		bool IsDirty() const;
		void SetDirty();
		void ResetDirty();

		void SetPath(const Path& path);

		void SetInstanceToSave(const void* instance, ReflectionDescriptor* reflectionDescriptor);

	private:
		bool _dirty = false;

		Meta _meta;

		String _name;
		Path   _path;

		renderer::Texture* _thumbnail = nullptr;

		const void*           _instanceToSave = nullptr;
		ReflectionDescriptor* _instanceToSaveReflectionDescriptor = nullptr;
	};
}
