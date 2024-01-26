#include "HodEngine/Core/ResourceManager.hpp"

#include "HodEngine/Core/Resource.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"

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

		std::ifstream stream(path);
		if (stream.is_open() == false)
		{
			return false; // todo message
		}

		constexpr size_t signatureLen = 11; // HodResource
		char signature[signatureLen];
		stream.read(signature, signatureLen);
		if (stream.fail()) // Read fail
		{
			return false; // todo message
		}
		if (std::strncmp(signature, "HodResource", signatureLen) != 0)
		{
			return false; // todo message
		}
		uint32_t documentLen = 0;
		stream.read((char*)(&documentLen), sizeof(documentLen));
		if (stream.fail())
		{
			return false; // todo message
		}

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, stream, documentLen) == false)
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

		return resource->Initialize(rootNode, stream);
	}
}
