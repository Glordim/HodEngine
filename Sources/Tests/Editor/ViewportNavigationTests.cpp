#include <gtest/gtest.h>

#include <HodEngine/Editor/Framing.hpp>
#include <HodEngine/Editor/ViewportNavigation.hpp>

namespace hod::viewport_navigation_tests
{
	constexpr int Left = 0;
	constexpr int Right = 1;
	constexpr int Middle = 2;

	/// @return The frame a button is pressed on, over the view
	static ViewportNavigation::Input Press(int button, const Vector2& position)
	{
		ViewportNavigation::Input input;
		input._hovered = true;
		input._down[button] = true;
		input._clicked[button] = true;
		input._mousePosition = position;
		return input;
	}

	/// @return A following frame, the button still down, the mouse moved by delta (the view hovered or not)
	static ViewportNavigation::Input Move(int button, const Vector2& position, const Vector2& delta, bool hovered = true)
	{
		ViewportNavigation::Input input;
		input._hovered = hovered;
		input._down[button] = true;
		input._mousePosition = position;
		input._mouseDelta = delta;
		return input;
	}

	TEST(ViewportNavigation, MiddleDragPans)
	{
		ViewportNavigation navigation;
		navigation.Update(Press(Middle, Vector2(10.0f, 10.0f)));
		EXPECT_TRUE(navigation.IsPanning());

		navigation.Update(Move(Middle, Vector2(15.0f, 7.0f), Vector2(5.0f, -3.0f)));
		EXPECT_FLOAT_EQ(navigation.GetPanDelta().GetX(), 5.0f);
		EXPECT_FLOAT_EQ(navigation.GetPanDelta().GetY(), -3.0f);
		EXPECT_FLOAT_EQ(navigation.GetZoomDelta(), 0.0f);
	}

	// The right button pans
	TEST(ViewportNavigation, RightDragPans)
	{
		ViewportNavigation navigation;
		navigation.Update(Press(Right, Vector2::Zero));
		EXPECT_TRUE(navigation.IsPanning());
	}

	// A one-button trackpad: Option+Cmd+click (Alt+Ctrl+left) pans
	TEST(ViewportNavigation, AltCtrlLeftDragPans)
	{
		ViewportNavigation        navigation;
		ViewportNavigation::Input input = Press(Left, Vector2::Zero);
		input._alt = true;
		input._ctrl = true;
		navigation.Update(input);
		EXPECT_TRUE(navigation.IsPanning());
	}

	// The left button is the view's (selection, handles) unless it says otherwise
	TEST(ViewportNavigation, PlainLeftDragPansOnlyWhenAllowed)
	{
		ViewportNavigation navigation;
		navigation.Update(Press(Left, Vector2::Zero));
		EXPECT_FALSE(navigation.IsDragging());

		ViewportNavigation::Input altOnly = Press(Left, Vector2::Zero);
		altOnly._alt = true;
		navigation.Update(altOnly);
		EXPECT_FALSE(navigation.IsDragging());

		ViewportNavigation          other;
		ViewportNavigation::Options options;
		options._leftButtonPans = true;
		other.Update(Press(Left, Vector2::Zero), options);
		EXPECT_TRUE(other.IsPanning());
	}

	TEST(ViewportNavigation, AltRightDragZoomsAroundItsStart)
	{
		ViewportNavigation        navigation;
		ViewportNavigation::Input input = Press(Right, Vector2(100.0f, 50.0f));
		input._alt = true;
		navigation.Update(input);
		EXPECT_TRUE(navigation.IsZooming());

		// Right zooms in, up (screen Y down) too
		navigation.Update(Move(Right, Vector2(140.0f, 50.0f), Vector2(ViewportNavigation::ZoomDragPixelsPerNotch, 0.0f)));
		EXPECT_FLOAT_EQ(navigation.GetZoomDelta(), 1.0f);
		EXPECT_FLOAT_EQ(navigation.GetZoomAnchor().GetX(), 100.0f);
		EXPECT_FLOAT_EQ(navigation.GetZoomAnchor().GetY(), 50.0f);
		EXPECT_FLOAT_EQ(navigation.GetPanDelta().GetX(), 0.0f);

		navigation.Update(Move(Right, Vector2(140.0f, 10.0f), Vector2(0.0f, -ViewportNavigation::ZoomDragPixelsPerNotch)));
		EXPECT_FLOAT_EQ(navigation.GetZoomDelta(), 1.0f);
	}

	// macOS: Ctrl+click is a right click, with ImGuiIO::MouseCtrlLeftAsRightClick: a zoom drag
	TEST(ViewportNavigation, MacCtrlClickDragZooms)
	{
		ViewportNavigation        navigation;
		ViewportNavigation::Input input = Press(Right, Vector2::Zero);
		input._ctrlClickAsRightClick = true;
		navigation.Update(input);
		EXPECT_TRUE(navigation.IsZooming());
	}

	TEST(ViewportNavigation, WheelZoomsAroundTheCursor)
	{
		ViewportNavigation        navigation;
		ViewportNavigation::Input input;
		input._hovered = true;
		input._wheel = 0.5f;
		input._mousePosition = Vector2(30.0f, 40.0f);
		navigation.Update(input);
		EXPECT_FLOAT_EQ(navigation.GetZoomDelta(), 0.5f);
		EXPECT_FLOAT_EQ(navigation.GetZoomAnchor().GetX(), 30.0f);
		EXPECT_FLOAT_EQ(navigation.GetZoomFactor(4.0f), 2.0f);

		ViewportNavigation::Options options;
		options._wheelZooms = false;
		navigation.Update(input, options);
		EXPECT_FLOAT_EQ(navigation.GetZoomDelta(), 0.0f);

		input._hovered = false;
		navigation.Update(input);
		EXPECT_FLOAT_EQ(navigation.GetZoomDelta(), 0.0f);
	}

	// Started over the view, a drag goes on outside of it until its button is released; it doesn't start outside of it
	TEST(ViewportNavigation, DragLastsUntilReleased)
	{
		ViewportNavigation navigation;
		navigation.Update(Press(Middle, Vector2::Zero));
		navigation.Update(Move(Middle, Vector2(-5.0f, 0.0f), Vector2(-5.0f, 0.0f), false));
		EXPECT_TRUE(navigation.IsPanning());
		EXPECT_FLOAT_EQ(navigation.GetPanDelta().GetX(), -5.0f);

		navigation.Update(ViewportNavigation::Input());
		EXPECT_FALSE(navigation.IsDragging());
		EXPECT_FLOAT_EQ(navigation.GetPanDelta().GetX(), 0.0f);

		ViewportNavigation::Input outside = Press(Middle, Vector2::Zero);
		outside._hovered = false;
		navigation.Update(outside);
		EXPECT_FALSE(navigation.IsDragging());
	}

	// Another button pressed during a drag doesn't take it over
	TEST(ViewportNavigation, OneDragAtATime)
	{
		ViewportNavigation navigation;
		navigation.Update(Press(Middle, Vector2::Zero));

		ViewportNavigation::Input input = Move(Middle, Vector2::Zero, Vector2::Zero);
		input._down[Right] = true;
		input._clicked[Right] = true;
		input._alt = true;
		navigation.Update(input);
		EXPECT_TRUE(navigation.IsPanning());
	}

	// The point under the anchor stays under it all along the eased zoom
	TEST(FramingCamera, ZoomKeepsTheAnchorStill)
	{
		FramingCamera camera;
		camera.Snap(Vector2(2.0f, -1.0f), 1.0f);

		Vector2 anchor(3.0f, 4.0f); // from the view's center, in units at zoom 1
		Vector2 pointUnderAnchor = camera.GetPosition() + anchor / camera.GetZoom();

		camera.ZoomBy(2.0f, anchor);
		for (int frame = 0; frame < 120; ++frame)
		{
			camera.Update(1.0f / 60.0f);
			Vector2 point = camera.GetPosition() + anchor / camera.GetZoom();
			EXPECT_NEAR(point.GetX(), pointUnderAnchor.GetX(), 1e-4f);
			EXPECT_NEAR(point.GetY(), pointUnderAnchor.GetY(), 1e-4f);
		}
		EXPECT_FLOAT_EQ(camera.GetZoom(), FramingCamera::ZoomPerNotch * FramingCamera::ZoomPerNotch);
	}

	// A notch in then a notch out gives back the zoom
	TEST(FramingCamera, ZoomNotchesAreSymmetric)
	{
		FramingCamera camera;
		camera.ZoomBy(1.0f);
		camera.ZoomBy(-1.0f);
		for (int frame = 0; frame < 120; ++frame)
		{
			camera.Update(1.0f / 60.0f);
		}
		EXPECT_FLOAT_EQ(camera.GetZoom(), 1.0f);
	}

	// The content follows the mouse: a pan of the view's pixels, whatever the zoom; the zoom around the anchor's screen position
	TEST(FramingCamera, Navigate)
	{
		FramingCamera camera;
		camera.Snap(Vector2::Zero, 2.0f);

		ViewportNavigation        navigation;
		ViewportNavigation::Input press;
		press._hovered = true;
		press._down[Middle] = true;
		press._clicked[Middle] = true;
		navigation.Update(press);

		ViewportNavigation::Input move;
		move._hovered = true;
		move._down[Middle] = true;
		move._mouseDelta = Vector2(20.0f, 10.0f); // right and down
		navigation.Update(move);

		const float unitScale = 10.0f; // 20 pixels a unit at zoom 2
		camera.Navigate(navigation, Vector2(100.0f, 100.0f), unitScale);
		EXPECT_FLOAT_EQ(camera.GetPosition().GetX(), -1.0f);
		EXPECT_FLOAT_EQ(camera.GetPosition().GetY(), 0.5f);

		// The wheel at 40 pixels right of the center: the point there (2 units right of the camera) stays there
		ViewportNavigation::Input wheel;
		wheel._hovered = true;
		wheel._wheel = 1.0f;
		wheel._mousePosition = Vector2(140.0f, 100.0f);
		navigation.Update(wheel);
		camera.Navigate(navigation, Vector2(100.0f, 100.0f), unitScale);
		Vector2 pointUnderCursor = camera.GetPosition() + Vector2(2.0f, 0.0f);
		for (int frame = 0; frame < 120; ++frame)
		{
			camera.Update(1.0f / 60.0f);
		}
		EXPECT_NEAR(camera.GetPosition().GetX() + 40.0f / (unitScale * camera.GetZoom()), pointUnderCursor.GetX(), 1e-4f);
		EXPECT_NEAR(camera.GetPosition().GetY(), pointUnderCursor.GetY(), 1e-4f);
	}
}
