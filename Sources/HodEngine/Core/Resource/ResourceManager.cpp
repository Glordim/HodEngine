#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Resource/Resource.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#include <cstring>
#include <fstream>

#undef FindResource

namespace hod
{
	/// @brief
	/// @param directory
	void ResourceManager::SetResourceDirectory(const std::filesystem::path& directory)
	{
		_directory = directory;
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
		std::filesystem::path path = _directory;
		path /= uid.ToString() + ".dat";

		FileSystem::Handle fileHandle = FileSystem::GetInstance()->Open(path);
		if (fileHandle.IsOpen() == false)
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

		constexpr size_t signatureLen = 11;
		char             signature[signatureLen];
		if (FileSystem::GetInstance()->Read(fileHandle, signature, signatureLen) != signatureLen)
		{
			FileSystem::GetInstance()->Close(fileHandle);
			return false; // todo message
		}
		if (std::strncmp(signature, "HodResource", signatureLen) != 0)
		{
			FileSystem::GetInstance()->Close(fileHandle);
			return false; // todo message
		}

		uint32_t version = 0;
		if (FileSystem::GetInstance()->Read(fileHandle, (char*)(&version), sizeof(version)) != sizeof(version))
		{
			FileSystem::GetInstance()->Close(fileHandle);
			return false; // todo message
		}
		if (version == 1)
		{
			uint32_t documentLen = 0;
			if (FileSystem::GetInstance()->Read(fileHandle, (char*)(&documentLen), sizeof(documentLen)) != sizeof(documentLen))
			{
				FileSystem::GetInstance()->Close(fileHandle);
				return false; // todo message
			}

			Document           document;
			DocumentReaderJson documentReader;
			if (documentReader.Read(document, fileHandle, documentLen) == false)
			{
				FileSystem::GetInstance()->Close(fileHandle);
				return false; // todo message
			}
			Document::Node& rootNode = document.GetRootNode();

			uint32_t dataCount = 0;
			if (FileSystem::GetInstance()->Read(fileHandle, (char*)(&dataCount), sizeof(dataCount)) != sizeof(dataCount))
			{
				FileSystem::GetInstance()->Close(fileHandle);
				return false; // todo message
			}
			Vector<Resource::Data> datas;
			datas.reserve(dataCount);
			for (uint32_t dataIndex = 0; dataIndex < dataCount; ++dataIndex)
			{
				uint32_t dataSize = 0;
				if (FileSystem::GetInstance()->Read(fileHandle, (char*)(&dataSize), sizeof(dataSize)) != sizeof(dataSize))
				{
					FileSystem::GetInstance()->Close(fileHandle);
					return false; // todo message
				}

				Resource::Data data;
				data._buffer = DefaultAllocator::GetInstance().Allocate(dataSize);
				data._size = dataSize;

				if (FileSystem::GetInstance()->Read(fileHandle, (char*)(data._buffer), dataSize) != (int32_t)dataSize)
				{
					FileSystem::GetInstance()->Close(fileHandle);
					return false; // todo message
				}

				datas.push_back(data);
			}
			FileSystem::GetInstance()->Close(fileHandle);

			bool result = resource->Initialize(rootNode, datas);

			for (const Resource::Data& data : datas)
			{
				DefaultAllocator::GetInstance().Free(data._buffer);
			}

			return result;
		}
		else
		{
			FileSystem::GetInstance()->Close(fileHandle);
			return false; // todo message
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

		std::shared_ptr<Resource> resource = reflectionDescriptor.CreateSharedInstance<Resource>();
		resource->SetUid(uid);
		if (Load(resource.get(), uid) == false)
		{
			return nullptr;
		}
		_resources[uid] = resource;
		return resource;
	}
}
