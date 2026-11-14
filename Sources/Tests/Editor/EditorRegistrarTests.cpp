#include <gtest/gtest.h>

#include <HodEngine/Core/ModuleHost.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp>
#include <HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp>
#include <HodEngine/Editor/EditorRegistrar.hpp>

namespace hod
{
	class EditorRegistrarTestType
	{
		REFLECTED_CLASS_NO_VIRTUAL(EditorRegistrarTestType)
	};

	DESCRIBE_REFLECTED_CLASS(EditorRegistrarTestType, reflectionDescriptor) {}

	namespace
	{
		class FirstDrawer : public CustomComponentDrawer
		{
		};

		class SecondDrawer : public CustomComponentDrawer
		{
		};
	}

	// Two modules registering a drawer for the same type: the second is refused (its drawer deleted), its undo leaves the first one's alone
	TEST(EditorRegistrar, RegisteredAlreadyIsRefusedTheFirstStays)
	{
		{
			ModuleHost host;
			EXPECT_TRUE(host.Start("First", [](ModuleRegistration& registration)
			                       { return EditorRegistrar(registration).RegisterCustomComponentDrawer<EditorRegistrarTestType, FirstDrawer>(); }));
			EXPECT_TRUE(host.Start("Second", [](ModuleRegistration& registration)
			                       { return EditorRegistrar(registration).RegisterCustomComponentDrawer<EditorRegistrarTestType, SecondDrawer>() == false; }));

			host.StopLast(1);
			EXPECT_NE(dynamic_cast<FirstDrawer*>(CustomComponentDrawerRegistry::Find<EditorRegistrarTestType>()), nullptr);
		}
		EXPECT_EQ(CustomComponentDrawerRegistry::Find<EditorRegistrarTestType>(), nullptr);
	}
}
