#include <gtest/gtest.h>

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>

using namespace hod;

// ============================================================================
// Matrix4 holds its translation in its last column and transforms column vectors: what it gives is in the clip space
// of the RHI, x and y in [-1, 1] with +y going up, the depth in [0, 1] from the nearest to the farthest.
// ============================================================================

namespace
{
	constexpr float Tolerance = 0.0001f;

	void ExpectClipPosition(const Matrix4& projection, const Vector4& position, float x, float y, float depth)
	{
		Vector4 clip = projection * position;

		EXPECT_NEAR(clip.GetX(), x, Tolerance);
		EXPECT_NEAR(clip.GetY(), y, Tolerance);
		EXPECT_NEAR(clip.GetZ(), depth, Tolerance);
		EXPECT_NEAR(clip.GetW(), 1.0f, Tolerance);
	}
}

// A box that is not centered on the camera: each of its sides goes to the matching side of the clip space
TEST(OrthogonalProjection, MapsTheSidesOfTheBoxToTheClipSpace)
{
	Matrix4 projection = Matrix4::OrthogonalProjection(10.0f, 30.0f, -5.0f, 15.0f, 1.0f, 101.0f);

	ExpectClipPosition(projection, Vector4(10.0f, -5.0f, -1.0f, 1.0f), -1.0f, -1.0f, 0.0f);
	ExpectClipPosition(projection, Vector4(30.0f, 15.0f, -101.0f, 1.0f), 1.0f, 1.0f, 1.0f);
	ExpectClipPosition(projection, Vector4(20.0f, 5.0f, -51.0f, 1.0f), 0.0f, 0.0f, 0.5f);
}

// The camera looks down -z: the greater the z, the nearer
TEST(OrthogonalProjection, GreaterZIsNearer)
{
	Matrix4 projection = Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	float back = (projection * Vector4(0.0f, 0.0f, -0.5f, 1.0f)).GetZ();
	float middle = (projection * Vector4(0.0f, 0.0f, 0.0f, 1.0f)).GetZ();
	float front = (projection * Vector4(0.0f, 0.0f, 0.5f, 1.0f)).GetZ();

	EXPECT_LT(front, middle);
	EXPECT_LT(middle, back);
}

// What 2D draws is at z = 0, which a box going as far behind the camera as in front of it holds in its middle
TEST(OrthogonalProjection, SymmetricDepthRangeKeepsZeroInside)
{
	ExpectClipPosition(Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.0f, 0.5f);
	ExpectClipPosition(Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1024.0f, 1024.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.0f, 0.5f);
}

TEST(OrthogonalProjection, ComposesWithTheViewAndModelMatrices)
{
	Matrix4 projection = Matrix4::OrthogonalProjection(0.0f, 10.0f, 0.0f, 10.0f, 0.0f, 10.0f);
	Matrix4 view = Matrix4::Inverse(Matrix4::Translation(Vector4(2.0f, 0.0f, 5.0f, 1.0f))); // the camera is at (2, 0, 5)
	Matrix4 model = Matrix4::Translation(Vector2(3.0f, 4.0f));

	// (3, 4, 0) in the world is (1, 4, -5) for the camera
	ExpectClipPosition(projection * view * model, Vector4(0.0f, 0.0f, 0.0f, 1.0f), -0.8f, -0.2f, 0.5f);
}

TEST(Matrix4, TranslationIgnoresTheWOfItsVector)
{
	Vector4 moved = Matrix4::Translation(Vector4(1.0f, 2.0f, 3.0f, 0.0f)) * Vector4(10.0f, 20.0f, 30.0f, 1.0f);

	EXPECT_NEAR(moved.GetX(), 11.0f, Tolerance);
	EXPECT_NEAR(moved.GetY(), 22.0f, Tolerance);
	EXPECT_NEAR(moved.GetZ(), 33.0f, Tolerance);
	EXPECT_NEAR(moved.GetW(), 1.0f, Tolerance);
}
