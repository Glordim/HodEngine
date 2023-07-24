#include "HodEngine/Game/Scene.h"

#include "HodEngine/Core/Reflection/ReflectionHelper.h"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS_NO_PARENT(Scene)
	{
	}

	/// @brief 
	/// @param documentNode 
	bool Scene::SerializeInDocument(Document::Node& documentNode) const
	{
		documentNode.Copy(_document.GetRootNode());
		return true;
	}

	/// @brief 
	/// @param documentNode 
	bool Scene::DeserializeFromDocument(const Document::Node& documentNode)
	{
		_document.GetRootNode().Copy(documentNode);
		return true;
	}
}
