#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/NodeFactory.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui2
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
