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
		DefaultAllocator::GetInstance().Delete(_prefab);
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool PrefabResource::Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas)
	{
		_document.GetRootNode().Copy(documentNode);

		_prefab = DefaultAllocator::GetInstance().New<Prefab>();
		bool result = _prefab->DeserializeFromDocument(documentNode);
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
