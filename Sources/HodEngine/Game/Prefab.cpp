#include "HodEngine/Game/Prefab.hpp"

#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS_NO_PARENT(Prefab)
	{
		AddTrait<ReflectionTraitCustomSerialization>(
		[](const void* instance, Document::Node& documentNode)
		{
			static_cast<const Prefab*>(instance)->SerializeInDocument(documentNode);
		},
		[](void* instance, const Document::Node& documentNode)
		{
			static_cast<Prefab*>(instance)->DeserializeFromDocument(documentNode);
		});
	}

	/// @brief 
	/// @param documentNode 
	bool Prefab::SerializeInDocument(Document::Node& documentNode) const
	{
		documentNode.Copy(_document.GetRootNode());
		return true;
	}

	/// @brief 
	/// @param documentNode 
	bool Prefab::DeserializeFromDocument(const Document::Node& documentNode)
	{
		_document.GetRootNode().Copy(documentNode);
		return true;
	}
}
