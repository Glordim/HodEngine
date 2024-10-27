#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(PrefabResource, Resource)
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
		return _prefab->DeserializeFromDocument(documentNode);
	}

	/// @brief 
	/// @return 
	Prefab& PrefabResource::GetPrefab()
	{
		return *_prefab;
	}
}
