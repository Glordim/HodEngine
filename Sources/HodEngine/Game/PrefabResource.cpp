#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(PrefabResource, Resource)
	{
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool PrefabResource::Initialize(const Document::Node& documentNode, std::istream& stream)
	{
		_prefab = new Prefab(GetUid());
		return _prefab->DeserializeFromDocument(documentNode);
	}

	/// @brief 
	void PrefabResource::Destroy()
	{
		delete _prefab;
		_prefab = nullptr;	
	}

	/// @brief 
	/// @return 
	Prefab& PrefabResource::GetPrefab()
	{
		return *_prefab;
	}
}
