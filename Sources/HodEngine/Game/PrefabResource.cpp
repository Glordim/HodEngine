#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(PrefabResource, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	PrefabResource::~PrefabResource()
	{
		DefaultAllocator::GetInstance().Delete(_prefab);
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool PrefabResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* entitiesDataBlock = resourceContainer.FindDataBlock("Entities");
		if (entitiesDataBlock == nullptr)
		{
			return false;
		}

		DocumentReaderJson documentReader;
		if (documentReader.Read(_document, *entitiesDataBlock->_stream) == false)
		{
			return false;
		}
		
		_prefab = DefaultAllocator::GetInstance().New<Prefab>();
		bool result = _prefab->DeserializeFromDocument(_document.GetRootNode());
		if (result == false)
		{
			DefaultAllocator::GetInstance().Delete(_prefab);
		}
		return result;
	}

	/// @brief 
	/// @return 
	Prefab& PrefabResource::GetPrefab()
	{
		return *_prefab;
	}

	/// @brief 
	/// @return 
	const Document& PrefabResource::GetDocument() const
	{
		return _document;
	}
}
