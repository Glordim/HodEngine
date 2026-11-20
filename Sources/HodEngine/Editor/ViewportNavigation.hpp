#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>

namespace hod::inline editor
{
	/// @brief The mouse controls moving the camera of the editor's 2D views (scene, prefab, UI prefab, texture, font...), every
	/// one reachable with a one-button trackpad:
	/// - pan: middle drag, right drag, Alt+Ctrl+left drag (Option+Cmd+click on macOS, Cmd being ImGui's Ctrl there), and a plain left
	///   drag in a view whose left button does nothing else (Options::_leftButtonPans: texture, font)
	/// - zoom: the wheel (a two-finger swipe on a trackpad) around the cursor; Alt+right drag, and Ctrl+left drag on macOS (a
	///   Ctrl+click is a right click there, ImGuiIO::MouseCtrlLeftAsRightClick), around where the drag started, right or up zooming in
	/// A drag starts on a press over the view and lasts until its button is released, wherever the mouse goes. From its press
	/// (IsDragging), the view's own use of the mouse (picking, selection, handles, the play mode's pointer) is off.
	///
	/// What the view does with the pan and the zoom is its own: FramingCamera::Navigate for the views having one.
	class HOD_EDITOR_API ViewportNavigation
	{
	public:
		/// @brief Pixels of a zoom drag making a wheel notch
		static constexpr float ZoomDragPixelsPerNotch = 40.0f;

		struct Options
		{
			bool _leftButtonPans = false; // A plain left drag pans too: a view without selection nor handle
			bool _wheelZooms = true;      // False when the wheel is the content's (a playing UI's scroll views)
		};

		/// @brief The mouse of a frame, as ImGuiIO gives it (Ctrl being Cmd on macOS)
		struct Input
		{
			bool    _hovered = false; // Over the view, not taken by something on it (a toolbar)
			bool    _down[3] = {false, false, false};    // ImGuiMouseButton_Left, Right, Middle
			bool    _clicked[3] = {false, false, false}; // Pressed this frame
			bool    _alt = false;
			bool    _ctrl = false;
			bool    _ctrlClickAsRightClick = false; // macOS: the right button is a Ctrl+left click
			float   _wheel = 0.0f;
			Vector2 _mousePosition = Vector2::Zero;
			Vector2 _mouseDelta = Vector2::Zero;
		};

	public:
		/// @brief Reads the mouse from ImGui, once per frame, before the view uses it. Shows the hand cursor while panning.
		/// @param hovered the mouse is over the view and not taken by something on it (a toolbar)
		void Update(bool hovered, const Options& options);
		void Update(bool hovered);

		/// @brief Update from a given input
		void Update(const Input& input, const Options& options);
		void Update(const Input& input);

		/// @brief A pan or zoom drag goes on (from its press): the view leaves the mouse alone
		bool IsDragging() const;
		bool IsPanning() const;
		bool IsZooming() const;

		/// @brief How much the content moves this frame, in screen pixels (Y down): it follows the mouse
		const Vector2& GetPanDelta() const;

		/// @brief Zoom of this frame, in wheel notches (> 0 zooms in), around GetZoomAnchor
		float GetZoomDelta() const;

		/// @brief Zoom factor of this frame for a view zooming by notchFactor a notch (1.1: 10% a notch)
		float GetZoomFactor(float notchFactor) const;

		/// @brief Screen position the zoom is around: the cursor for the wheel, the drag's start for a zoom drag
		const Vector2& GetZoomAnchor() const;

	private:
		enum class Drag : uint8_t
		{
			None,
			Pan,
			Zoom,
		};

		Drag    _drag = Drag::None;
		uint8_t _button = 0;
		Vector2 _dragStart = Vector2::Zero;

		Vector2 _panDelta = Vector2::Zero;
		float   _zoomDelta = 0.0f;
		Vector2 _zoomAnchor = Vector2::Zero;
	};
}
