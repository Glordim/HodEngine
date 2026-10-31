#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Time/SystemTime.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <cstdint>
#include <functional>

namespace hod::inline ui2
{
	class Canvas;

	/// @brief The game's canvases: advanced and laid out once per frame, and given the game's pointer. UI2 knows neither the
	/// game's view nor its cameras: the pointer is fed by whoever owns the view (SetPointer: the application from the window's
	/// mouse, the editor from its game viewport), and each canvas taking it says how a view position maps into it (PointerMapping,
	/// e.g. through the camera for a canvas placed in the world).
	class HOD_UI2_API CanvasManager final
	{
		_Singleton(CanvasManager)

	public:

		// A position in the view (pixels, origin at its top-left corner, y down) to the canvas' space. false when the canvas
		// can't take the pointer (not displayed in that view...).
		using PointerMapping = std::function<bool(const Vector2& viewPosition, const Vector2& viewSize, Vector2& outCanvasPosition)>;

	public:

		void	AddCanvas(Canvas* canvas);
		void	RemoveCanvas(Canvas* canvas);

		// canvas (added) takes the pointer through mapping, before the canvases of a lower priority: the first one whose nodes
		// catch the pointer (see Canvas::UpdatePointer) takes it, the others lose it. A null mapping takes the pointer away.
		void	SetPointerMapping(Canvas* canvas, PointerMapping mapping, int64_t priority);

		// The pointer over the view this frame (position: see PointerMapping), with its primary button's state (left mouse
		// button, touch). Without a call in a frame, there is no pointer (out of the view, or not playing).
		void	SetPointer(const Vector2& viewPosition, const Vector2& viewSize, bool pressed);

		// Whether the pointer was over a canvas' node catching it, or pressing one of them, at the last Update(): the input is
		// the UI's, the game shouldn't act on what is under it.
		bool	IsPointerOverUI() const;

		// Advances (see Canvas::Update) then lays out every canvas, once per frame, then gives them the pointer.
		void	Update();

	private:

		struct Entry
		{
			Canvas*			_canvas = nullptr;
			PointerMapping	_pointerMapping;
			int64_t			_priority = 0;
			bool			_pressed = false; // The press in progress started over it: only then does it see the button down
		};

		Entry*	FindEntry(Canvas* canvas);
		void	UpdatePointer();

	private:

		Vector<Entry>	_entries; // By decreasing priority

		bool			_hasPointer = false;
		Vector2			_pointerPosition;
		Vector2			_viewSize;
		bool			_pointerPressed = false;
		bool			_previousPointerPressed = false;
		bool			_pointerOverUI = false;

		SystemTime::TimeStamp _lastUpdateTimestamp = SystemTime::INVALID_TIMESTAMP;
		bool                  _updatedOnce = false; // TimeStamp has no comparison on every platform
	};
}
