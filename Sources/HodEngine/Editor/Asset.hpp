#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Editor/Importer/Importer.hpp"
#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline renderer
{
	class Texture;
}

namespace hod::inline editor
{
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

		uint64_t GetType() const { return _assetType; }
		uint64_t GetContentHash() const { return _contentHash; }

		bool HasSource() const { return _sourcePath.Empty() == false; }
		const Path& GetSourcePath() const { return _sourcePath; }

		Texture* GetThumbnail() const;

		bool IsDirty() const;
		void SetDirty();
		void ResetDirty();

		void SetPath(const Path& path);

		void SetInstanceToSave(const void* instance, ReflectionDescriptor* reflectionDescriptor);

	private:
		bool _dirty = false;

		String _name;
		Path _path;
		UID _uid;
		uint64_t _assetType = 0;
		uint64_t _contentHash = 0;
		Path _sourcePath;
		uint64_t _sourceHash = 0;

		Texture* _thumbnail = nullptr;

		const void*           _instanceToSave = nullptr;
		ReflectionDescriptor* _instanceToSaveReflectionDescriptor = nullptr;
	};
}
