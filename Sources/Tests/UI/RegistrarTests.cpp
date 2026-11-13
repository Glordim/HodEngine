#include <gtest/gtest.h>

#include <HodEngine/Core/ModuleHost.hpp>
#include <HodEngine/UI/Node.hpp>
#include <HodEngine/UI/NodeFactory.hpp>
#include <HodEngine/UI/UIRegistrar.hpp>

namespace hod
{
	class RegistrarTestNode : public Node
	{
		REFLECTED_CLASS(RegistrarTestNode, Node)
	};

	DESCRIBE_REFLECTED_CLASS(RegistrarTestNode, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	namespace
	{
		bool IsNodeRegistered()
		{
			return NodeFactory::GetInstance()->GetAllDescriptors().contains(RegistrarTestNode::GetReflectionDescriptor().GetType());
		}
	}

	// Two modules registering the same node type: the second is refused, its undo leaves the first one's alone
	TEST(UIRegistrar, RegisteredAlreadyIsRefusedTheFirstStays)
	{
		NodeFactory::CreateInstance();
		{
			ModuleHost host;
			EXPECT_TRUE(host.Start("First", [](ModuleRegistration& registration) { return UIRegistrar(registration).RegisterNode<RegistrarTestNode>(); }));
			EXPECT_TRUE(host.Start("Second", [](ModuleRegistration& registration) { return UIRegistrar(registration).RegisterNode<RegistrarTestNode>() == false; }));

			host.StopLast(1);
			EXPECT_TRUE(IsNodeRegistered());
		}
		EXPECT_FALSE(IsNodeRegistered());
		NodeFactory::DestroyInstance();
	}
}
