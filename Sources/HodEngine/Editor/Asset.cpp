#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/Texture.hpp"
#include "HodEngine/Renderer/FrameResources.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

namespace hod::inline editor
{
	/// @brief
	Asset::Asset(const Path& path)
	: _path(path)
	{
		_name = _path.Stem().GetString();
		_uid = UID::GenerateUID();
	}

	/// @brief
	Asset::~Asset()
	{
		ReleaseThumbnail();
	}

	/// @brief The thumbnail may already be referenced by this frame's ImGui draw lists (Load() runs from the
	/// AssetDatabase PostRender job, before Renderer::Render()), so its deletion waits for the frame to be done
	void Asset::ReleaseThumbnail() const
	{
		if (_thumbnail != nullptr)
		{
			Renderer* renderer = Renderer::GetInstance();
			if (renderer != nullptr)
			{
				renderer->GetCurrentFrameResources().DeleteAfter(_thumbnail);
			}
			else
			{
				DefaultAllocator::GetInstance().Delete(_thumbnail);
			}
			_thumbnail = nullptr;
		}
		_thumbnailLoaded = false;
	}

	/// @brief
	/// @param path
	void Asset::SetPath(const Path& path)
	{
		_path = path;
		_name = path.Stem().GetString();
	}

	/// @brief
	/// @return
	bool Asset::Load()
	{
		AssetContainer assetContainer;
		if (assetContainer.Load(_path) == false)
		{
			if (assetContainer.HasValidHeaderMagic() == true)
			{
				OUTPUT_ERROR("Asset::Load: Unable to load AssetContainer from {}", _path);
			}
			return false;
		}

		_uid = assetContainer.GetUid();
		_assetType = assetContainer.GetAssetType();
		_subType = assetContainer.GetSubType();
		_contentHash = assetContainer.GetContentHash();
		_sources = assetContainer.GetSources();

		ReleaseThumbnail();

		/*
		Path metaPath = _path;
		metaPath += ".meta";

		if (FileSystem::GetInstance()->Exists(metaPath) == false)
		{
			// TODO generate new meta if not exist
			if (AssetDatabase::GetInstance()->Import(_path) == false)
			{
				return false;
			}
			if (FileSystem::GetInstance()->Exists(metaPath) == false)
			{
				return false;
			}
		}

		Document           document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, metaPath) == false)
		{
			return false;
		}

		if (Serializer::Deserialize(_meta, document.GetRootNode()) == false)
		{
			return false;
		}
		const DocumentNode* importerSettingsNode = document.GetRootNode().GetChild("importerSettings");
		if (importerSettingsNode != nullptr && _meta.LoadImporterConfig(*importerSettingsNode) == false) // TODO improve reflection
		{
			return false;
		}

		AssetDatabase::GetInstance()->ReimportAssetIfNecessary(shared_from_this());
		*/

		return true;
	}

	/// @brief
	/// @param instance
	/// @param reflectionDescriptor
	/// @return
	bool Asset::Save(const void* instance, ReflectionDescriptor* reflectionDescriptor)
	{
		(void)instance;
		(void)reflectionDescriptor;
		return false;
	}

	/// @brief
	/// @return
	const UID& Asset::GetUid() const
	{
		return _uid;
	}

	/// @brief
	/// @return
	const Path& Asset::GetPath() const
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
	Texture* Asset::GetThumbnail() const
	{
		if (_thumbnailLoaded == false)
		{
			_thumbnailLoaded = true; // Don't retry each frame on assets without thumbnail

			AssetContainer assetContainer;
			if (assetContainer.LoadThumbnail(_path))
			{
				_thumbnail = Thumbnail::CreateTexture(assetContainer.GetThumbnail(), assetContainer.GetThumbnailFilterMode());
			}
		}
		return _thumbnail;
	}
}
