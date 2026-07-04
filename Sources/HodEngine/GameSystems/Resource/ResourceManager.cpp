#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"

#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include "HodEngine/GameSystems/Resource/ResourceVariant.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <cstring>

#undef FindResource

namespace hod::inline gamesystems
{
	/// @brief
	/// @param directory
	void ResourceManager::SetResourceDirectory(const Path& directory)
	{
#if defined(ANDROID)
		_directory = "";
#else
		_directory = directory;
#endif
	}

	/// @brief
	/// @param uid
	/// @return
	std::shared_ptr<Resource> ResourceManager::FindResource(const UID& uid) const
	{
		auto it = _resources.find(uid);
		if (it != _resources.end())
		{
			std::shared_ptr<Resource> existingResource = it->second.lock();
			return existingResource;
		}
		return nullptr;
	}

	/// @brief
	/// @param resource
	/// @param uid
	/// @return
	bool ResourceManager::Load(Resource* resource, const UID& uid)
	{
		Path path = _directory / ResourceVariant::UnlocalizedName;
		path /= (uid.ToString() + ".res");

		if (_fileNotFoundCallback != nullptr)
		{
			if (FileSystem::GetInstance()->Exists(path) == false)
			{
				if (_fileNotFoundCallback(uid) == false)
				{
					return false;
				}
			}
		}

		ResourceContainer resourceContainer;
		if (resourceContainer.LoadHeader(path) == false)
		{
			return false;
		}

		if (_checkUpToDateCallback != nullptr)
		{
			if (_checkUpToDateCallback(uid, resourceContainer) == false)
			{
				return false;
			}
		}

		if (resourceContainer.Load(path) == false)
		{
			return false;
		}

		bool result = resource->Initialize(resourceContainer);
		if (result == false)
		{
			OUTPUT_ERROR("ResourceManager::Load: resource {} initialize failed", uid.ToString());
		}

		return result;
	}

	/// @brief
	/// @param reflectionDescriptor
	/// @param uid
	/// @return
	std::shared_ptr<Resource> ResourceManager::GetResource(const ReflectionDescriptor& reflectionDescriptor, const UID& uid)
	{
		std::shared_ptr<Resource> existingResource = FindResource(uid);
		if (existingResource != nullptr)
		{
			return existingResource;
		}

		std::shared_ptr<Resource> resource(reflectionDescriptor.CreateInstance<Resource>(), [](Resource* resource)
		{
			resource->GetReflectionDescriptorV().DeleteInstance(resource);
		});
		resource->SetUid(uid);
		if (Load(resource.get(), uid) == false)
		{
			return nullptr;
		}
		_resources[uid] = resource;
		return resource;
	}
}
