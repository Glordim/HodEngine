#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"

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
		path /= (uid.ToString() + ".dat").CStr(); // todo remove Cstr when remove Path

		FileStream fileStream;
		if (fileStream.Open(path) == false)
		{
			OUTPUT_ERROR("ResourceManager::Load: resource {} not found", uid.ToString());
			return false;
		}

		// Resource format (Version 1)
		//
		// HodResource (signature)
		// Version
		// DocumentLen
		// Document
		// DataCount
		// [
		//    DataSize
		//    Data
		// ]
		//

		constexpr uint32_t signatureLen = 11;
		char               signature[signatureLen];
		if (fileStream.Read(signature, signatureLen) != signatureLen)
		{
			OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read signature", uid.ToString());
			return false;
		}
		if (std::strncmp(signature, "HodResource", signatureLen) != 0)
		{
			OUTPUT_ERROR("ResourceManager::Load: resource {} hasn't HodResource signature", uid.ToString());
			return false;
		}

		uint32_t version = 0;
		if (fileStream.Read(&version, sizeof(version)) != sizeof(version))
		{
			OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read version", uid.ToString());
			return false;
		}
		if (version == 1)
		{
			uint32_t documentLen = 0;
			if (fileStream.Read(&documentLen, sizeof(documentLen)) != sizeof(documentLen))
			{
				OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read documentLen", uid.ToString());
				return false;
			}

			Document           document;
			DocumentReaderJson documentReader;
			if (documentReader.Read(document, fileStream, documentLen) == false)
			{
				OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read document", uid.ToString());
				return false;
			}
			DocumentNode& rootNode = document.GetRootNode();

			uint32_t dataCount = 0;
			if (fileStream.Read(&dataCount, sizeof(dataCount)) != sizeof(dataCount))
			{
				OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read dataCount", uid.ToString());
				return false;
			}
			Vector<Resource::Data> datas;
			datas.Reserve(dataCount);
			for (uint32_t dataIndex = 0; dataIndex < dataCount; ++dataIndex)
			{
				uint32_t dataSize = 0;
				if (fileStream.Read(&dataSize, sizeof(dataSize)) != sizeof(dataSize))
				{
					OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read data size {}", uid.ToString(), dataIndex);
					return false;
				}

				Resource::Data data;
				data._buffer = DefaultAllocator::GetInstance().Allocate(dataSize);
				data._size = dataSize;

				if (fileStream.Read(data._buffer, dataSize) != dataSize)
				{
					OUTPUT_ERROR("ResourceManager::Load: resource {} unable to read data {}", uid.ToString(), dataIndex);
					return false;
				}

				datas.push_back(data);
			}

			bool result = resource->Initialize(rootNode, datas);
			if (result == false)
			{
				OUTPUT_ERROR("ResourceManager::Load: resource {} initialize failed", uid.ToString());
			}

			for (const Resource::Data& data : datas)
			{
				DefaultAllocator::GetInstance().Free(data._buffer);
			}

			return result;
		}
		else
		{
			OUTPUT_ERROR("ResourceManager::Load: unsupported version {}", uid.ToString(), version);
			return false;
		}
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
