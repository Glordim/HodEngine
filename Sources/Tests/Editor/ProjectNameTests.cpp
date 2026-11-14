#include <gtest/gtest.h>

#include <HodEngine/Editor/Project.hpp>

namespace hod
{
	// The project's modules are named after it (<Name>, <Name>Editor): a C identifier, neither a library next to the editor nor its macros
	TEST(ProjectName, ModulesNamedAfterIt)
	{
		EXPECT_EQ(Project::GetRuntimeModuleName("Circle"), "Circle");
		EXPECT_EQ(Project::GetEditorModuleName("Circle"), "CircleEditor");

		EXPECT_TRUE(Project::CheckName("Circle").Empty());
		EXPECT_TRUE(Project::CheckName("HodGame").Empty());

		EXPECT_FALSE(Project::CheckName("").Empty());
		EXPECT_FALSE(Project::CheckName("1Circle").Empty());
		EXPECT_FALSE(Project::CheckName("Circle-2").Empty());
		EXPECT_FALSE(Project::CheckName("Hod").Empty());
		EXPECT_FALSE(Project::CheckName("hod_Circle").Empty());
		EXPECT_FALSE(Project::CheckName("Game").Empty());    // Game, next to the tests as next to the editor
		EXPECT_FALSE(Project::CheckName("Physics").Empty()); // PhysicsEditor
	}
}
