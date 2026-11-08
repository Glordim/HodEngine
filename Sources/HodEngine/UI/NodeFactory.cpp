#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/NodeFactory.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	_SingletonConstructor(NodeFactory)
	{

	}

	Node* NodeFactory::CreateNode(const ReflectionDescriptor& descriptor) const
	{
		Node* node = descriptor.CreateInstance<Node>();
		node->SetName(descriptor.GetDisplayName());
		return node;
	}
}
