#include <gtest/gtest.h>

#include <HodEngine/Core/CharHelper.hpp>

namespace hod
{
	TEST(GenerateDisplayName, Words)
	{
		EXPECT_EQ(GenerateDisplayName("velocity"), "Velocity");
		EXPECT_EQ(GenerateDisplayName("_velocityScale"), "Velocity Scale");
		EXPECT_EQ(GenerateDisplayName("SpriteRendererComponent"), "Sprite Renderer Component");
		EXPECT_EQ(GenerateDisplayName("max_speed"), "Max Speed");
		EXPECT_EQ(GenerateDisplayName(""), "");
	}

	// A run of capitals is an acronym, its last one starting the next word when a lower case letter follows
	TEST(GenerateDisplayName, Acronyms)
	{
		EXPECT_EQ(GenerateDisplayName("UIComponent"), "UI Component");
		EXPECT_EQ(GenerateDisplayName("HTTPRequest"), "HTTP Request");
		EXPECT_EQ(GenerateDisplayName("_isUI"), "Is UI");
		EXPECT_EQ(GenerateDisplayName("RHI"), "RHI");
	}

	// A number is a word, a single letter right after it its unit
	TEST(GenerateDisplayName, Numbers)
	{
		EXPECT_EQ(GenerateDisplayName("Rigidbody2dComponent"), "Rigidbody 2D Component");
		EXPECT_EQ(GenerateDisplayName("Node2d"), "Node 2D");
		EXPECT_EQ(GenerateDisplayName("Box2DShape"), "Box 2D Shape");
		EXPECT_EQ(GenerateDisplayName("Vector2"), "Vector 2");
		EXPECT_EQ(GenerateDisplayName("Vector2Int"), "Vector 2 Int");
		EXPECT_EQ(GenerateDisplayName("_layer10"), "Layer 10");
		EXPECT_EQ(GenerateDisplayName("_2ndPass"), "2nd Pass");
	}
}
