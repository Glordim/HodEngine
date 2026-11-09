#include <gtest/gtest.h>

#include <HodEngine/RendererComponents/DebugShapes.hpp>

// No GPU: the lifetime of the debug shapes of a world (RendererWorldSystem ages them once per update)
namespace hod
{
	TEST(DebugShapes, ZeroDurationIsDrawnOnce)
	{
		DebugShapes shapes;
		shapes.Line(Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Color(1.0f, 0.0f, 0.0f, 1.0f));

		shapes.Age(0.016f); // the update that added it: not drawn yet, kept
		EXPECT_EQ(shapes.GetShapes().GetShapes().Size(), 1u);

		shapes.Age(0.016f); // the next one: drawn once, removed
		EXPECT_TRUE(shapes.GetShapes().Empty());
		EXPECT_TRUE(shapes.GetShapes().GetPoints().Empty());
	}

	TEST(DebugShapes, AddedAfterAgeInTheFrameIsKeptUntilTheNextOne)
	{
		DebugShapes shapes;
		shapes.Age(0.016f);
		shapes.Line(Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Color(1.0f, 0.0f, 0.0f, 1.0f)); // by a system updated after

		shapes.Age(0.016f);
		EXPECT_EQ(shapes.GetShapes().GetShapes().Size(), 1u);
		shapes.Age(0.016f);
		EXPECT_TRUE(shapes.GetShapes().Empty());
	}

	TEST(DebugShapes, AgeKeepsTheOthersInOrderWithTheirPoints)
	{
		DebugShapes shapes;
		shapes.Line(Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Color(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
		shapes.Rect(Vector2(5.0f, 5.0f), Vector2(2.0f, 2.0f), Color(0.0f, 1.0f, 0.0f, 1.0f)); // for a single frame
		shapes.Circle(Vector2(3.0f, 4.0f), 0.5f, Color(0.0f, 0.0f, 1.0f, 1.0f), 0.5f);
		shapes.Point(Vector2(7.0f, 8.0f), 4.0f, Color(1.0f, 1.0f, 0.0f, 1.0f), 2.0f);
		shapes.Age(0.0f); // drawn once from here

		shapes.Age(0.25f);

		const Vector<ShapeList::Shape>& kept = shapes.GetShapes().GetShapes();
		const Vector<Vector2>&          points = shapes.GetShapes().GetPoints();
		ASSERT_EQ(kept.Size(), 3u);
		ASSERT_EQ(shapes.GetDurations().Size(), 3u);
		EXPECT_EQ(points.Size(), 4u); // 2 for the line, 1 for the circle, 1 for the point

		EXPECT_EQ(kept[0]._type, ShapeList::Type::Polyline);
		EXPECT_FLOAT_EQ(shapes.GetDurations()[0], 0.75f);
		EXPECT_EQ(points[kept[0]._firstPoint + 1], Vector2(1.0f, 0.0f));

		EXPECT_EQ(kept[1]._type, ShapeList::Type::Circle);
		EXPECT_FLOAT_EQ(kept[1]._size, 0.5f);
		EXPECT_EQ(points[kept[1]._firstPoint], Vector2(3.0f, 4.0f));

		EXPECT_EQ(kept[2]._type, ShapeList::Type::Point);
		EXPECT_EQ(points[kept[2]._firstPoint], Vector2(7.0f, 8.0f));

		shapes.Age(0.5f);
		ASSERT_EQ(shapes.GetShapes().GetShapes().Size(), 2u);
		EXPECT_EQ(shapes.GetShapes().GetShapes()[1]._type, ShapeList::Type::Point);
		EXPECT_EQ(shapes.GetShapes().GetPoints()[shapes.GetShapes().GetShapes()[1]._firstPoint], Vector2(7.0f, 8.0f));
	}

	TEST(DebugShapes, DegenerateShapesAreIgnored)
	{
		DebugShapes shapes;
		Vector2     point(1.0f, 1.0f);
		shapes.Polyline(&point, 1, false, Color(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
		shapes.FilledPolygon(&point, 1, Color(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
		shapes.Circle(point, 1.0f, Color(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
		EXPECT_EQ(shapes.GetShapes().GetShapes().Size(), 1u);
		ASSERT_EQ(shapes.GetDurations().Size(), 1u); // one duration per shape kept
		EXPECT_FLOAT_EQ(shapes.GetDurations()[0], 2.0f);
	}
}
