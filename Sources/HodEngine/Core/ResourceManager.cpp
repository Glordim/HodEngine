#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/ResourceManager.hpp"

#include "HodEngine/Core/Resource.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <fstream>
#include <cstring>

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
			return false; // todo message
		}

		constexpr size_t signatureLen = 11; // HodResource
		char signature[signatureLen];
		if (FileSystem::GetInstance()->Read(fileHandle, signature, signatureLen) != signatureLen)
		{
			return false; // todo message
		}
		if (std::strncmp(signature, "HodResource", signatureLen) != 0)
		{
			return false; // todo message
		}
		uint32_t documentLen = 0;
		if (FileSystem::GetInstance()->Read(fileHandle, (char*)(&documentLen), sizeof(documentLen)) != sizeof(documentLen))
		{
			return false; // todo message
		}

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, fileHandle, documentLen) == false)
		{
			return false; // todo message
		}

		Document::Node& rootNode = document.GetRootNode();
		/*
		const Document::Node* internalNode = rootNode.GetChild("Internal");
		if (internalNode == nullptr)
		{
			return false; // todo message
		}
		const Document::Node* streamOffsetNode = internalNode->GetChild("StreamOffset");
		if (streamOffsetNode == nullptr)
		{
			return false; // todo message
		}
		uint32_t streamOffset = streamOffsetNode->GetUInt32();
		const Document::Node* streamSizeNode = internalNode->GetChild("StreamSize");
		if (streamSizeNode == nullptr)
		{
			return false; // todo message
		}
		uint32_t streamSize = streamSizeNode->GetUInt32();

		stream.seekg(streamOffset, std::ios::beg);
		const Document::Node* contentNode = rootNode.GetChild("Content");
		if (contentNode == nullptr)
		{
			return false; // todo message
		}
		*/

		return resource->Initialize(rootNode, fileHandle);
	}

	/// @brief 
	/// @param reflectionDescriptor 
	/// @param uid 
	/// @return 
	std::shared_ptr<Resource> ResourceManager::GetResource(ReflectionDescriptor* reflectionDescriptor, const UID& uid)
	{
		std::shared_ptr<Resource> existingResource = FindResource(uid);
		if (existingResource != nullptr)
		{
			return existingResource;
		}

		std::shared_ptr<Resource> resource = reflectionDescriptor->CreateSharedInstance<Resource>();
		resource->SetUid(uid);
		if (Load(resource.get(), uid) == false)
		{
			return nullptr;
		}
		_resources[uid] = resource;
		return resource;
	}
}
