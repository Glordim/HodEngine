#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <cstdint>
#include <stdint.h>

#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
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

		const UID&    GetUid() const;
		const Path&   GetPath() const;
		const String& GetName() const;

		uint64_t GetType() const { return _assetType; }
		uint64_t GetContentHash() const { return _contentHash; }

		bool HasSources() const { return _sources.Empty() == false; }
		const Vector<AssetContainer::SourceInfo>& GetSources() const { return _sources; }

		Texture* GetThumbnail() const;

		void SetPath(const Path& path);

	private:
		String _name;
		Path _path;
		UID _uid;
		uint64_t _assetType = 0;
		uint64_t _contentHash = 0;
		Vector<AssetContainer::SourceInfo> _sources;

		Texture* _thumbnail = nullptr;
	};
}
