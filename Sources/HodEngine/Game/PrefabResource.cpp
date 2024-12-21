#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(PrefabResource, reflectionDescriptor)
	{
	}

	/// @brief 
	PrefabResource::~PrefabResource()
	{
		delete _prefab;
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool PrefabResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		_prefab = new Prefab();
		bool result = _prefab->DeserializeFromDocument(documentNode);
		if (result)
		{
			_prefab->GetRootEntity()->SetPrefabResource(std::static_pointer_cast<PrefabResource>(shared_from_this()));
		}
		return result;
	}

	/// @brief 
	/// @return 
	Prefab& PrefabResource::GetPrefab()
	{
		return *_prefab;
	}
}
