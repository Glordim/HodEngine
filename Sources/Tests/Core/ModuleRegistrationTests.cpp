#include <gtest/gtest.h>

#include <HodEngine/Core/ModuleRegistration.hpp>

#include <string>
#include <vector>

namespace hod
{
	TEST(ModuleRegistration, UnregistersInTheReverseOrder)
	{
		std::vector<std::string> calls;
		{
			ModuleRegistration registration;
			registration.AddUnregister([&calls]() { calls.push_back("first"); });
			registration.AddUnregister([&calls]() { calls.push_back("second"); });
			registration.AddUnregister([&calls]() { calls.push_back("third"); });
		}
		std::vector<std::string> expected {"third", "second", "first"};
		EXPECT_EQ(calls, expected);
	}

	TEST(ModuleRegistration, UnregisterAllRunsEachOnce)
	{
		int                count = 0;
		ModuleRegistration registration;
		registration.AddUnregister([&count]() { ++count; });
		registration.UnregisterAll();
		EXPECT_EQ(count, 1);

		// Usable again (registrations redone, as on a reload)
		registration.AddUnregister([&count]() { ++count; });
		registration.UnregisterAll();
		registration.UnregisterAll();
		EXPECT_EQ(count, 2);
	}
}
