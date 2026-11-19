#include <gtest/gtest.h>

#include <HodEngine/Editor/Gizmos/Handles.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <numbers>

namespace hod::handles_tests
{
	/// @return Where the local delta goes in the world through worldMatrix (as the handles are drawn)
	static Vector2 LocalToWorldDelta(const Matrix4& worldMatrix, const Vector2& localDelta)
	{
		Vector4 origin = worldMatrix * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		Vector4 end = worldMatrix * Vector4(localDelta.GetX(), localDelta.GetY(), 0.0f, 1.0f);
		return Vector2(end.GetX() - origin.GetX(), end.GetY() - origin.GetY());
	}

	TEST(Handles, WorldToLocalDeltaIdentity)
	{
		Vector2 local = Handles::WorldToLocalDelta(Matrix4::Identity, Vector2(1.5f, -2.0f));
		EXPECT_FLOAT_EQ(local.GetX(), 1.5f);
		EXPECT_FLOAT_EQ(local.GetY(), -2.0f);
	}

	// A drag on a rotated and scaled node: the local value moves so that the handle follows the mouse
	TEST(Handles, WorldToLocalDeltaUndoesRotationAndScaleNotTranslation)
	{
		Matrix4 worldMatrix = Matrix4::Translation(Vector2(10.0f, -3.0f)) * Matrix4::Rotation(std::numbers::pi_v<float> * 0.5f) * Matrix4::Scale(Vector2(2.0f, 0.5f));

		Vector2 localDelta(1.0f, 4.0f);
		Vector2 worldDelta = LocalToWorldDelta(worldMatrix, localDelta);
		EXPECT_GT(std::abs(worldDelta.GetX() - localDelta.GetX()) + std::abs(worldDelta.GetY() - localDelta.GetY()), 0.5f); // Not the same in the world

		Vector2 local = Handles::WorldToLocalDelta(worldMatrix, worldDelta);
		EXPECT_NEAR(local.GetX(), localDelta.GetX(), 1e-4f);
		EXPECT_NEAR(local.GetY(), localDelta.GetY(), 1e-4f);
	}

	TEST(Handles, ProjectOnAxis)
	{
		Vector2 alongX = Handles::ProjectOnAxis(Vector2(3.0f, 4.0f), Vector2(2.0f, 0.0f)); // Any length
		EXPECT_FLOAT_EQ(alongX.GetX(), 3.0f);
		EXPECT_FLOAT_EQ(alongX.GetY(), 0.0f);

		Vector2 diagonal = Handles::ProjectOnAxis(Vector2(2.0f, 0.0f), Vector2(1.0f, 1.0f));
		EXPECT_FLOAT_EQ(diagonal.GetX(), 1.0f);
		EXPECT_FLOAT_EQ(diagonal.GetY(), 1.0f);

		Vector2 free = Handles::ProjectOnAxis(Vector2(3.0f, 4.0f), Vector2::Zero); // No axis: unchanged
		EXPECT_FLOAT_EQ(free.GetX(), 3.0f);
		EXPECT_FLOAT_EQ(free.GetY(), 4.0f);
	}

	TEST(Handles, HighlightColorIsLighterSameAlpha)
	{
		Color highlight = Handles::GetHighlightColor(Color(1.0f, 0.0f, 0.2f, 0.5f));
		EXPECT_FLOAT_EQ(highlight.r, 1.0f);
		EXPECT_FLOAT_EQ(highlight.g, 0.5f);
		EXPECT_FLOAT_EQ(highlight.b, 0.6f);
		EXPECT_FLOAT_EQ(highlight.a, 0.5f);
	}

	// Made from a color: hovered and pressed lightened; pressed shown before hovered
	TEST(Handles, StyleColors)
	{
		HandleStyle simple = Color(1.0f, 0.0f, 0.0f, 1.0f);
		EXPECT_FLOAT_EQ(simple.GetColor(false, false).g, 0.0f);
		EXPECT_FLOAT_EQ(simple.GetColor(true, false).g, 0.5f);
		EXPECT_FLOAT_EQ(simple.GetColor(false, true).g, 0.5f);
		EXPECT_EQ(simple._sortingOrder, HandleStyle::DefaultSortingOrder);

		HandleStyle custom(Color(0.1f, 0.0f, 0.0f, 1.0f), Color(0.2f, 0.0f, 0.0f, 1.0f), Color(0.3f, 0.0f, 0.0f, 1.0f));
		EXPECT_FLOAT_EQ(custom.GetColor(false, false).r, 0.1f);
		EXPECT_FLOAT_EQ(custom.GetColor(true, false).r, 0.2f);
		EXPECT_FLOAT_EQ(custom.GetColor(true, true).r, 0.3f);
	}
}
