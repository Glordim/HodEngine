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

	/// @brief The game's canvases: advanced and laid out once per frame, and given the game's pointer. The pointer is read by
	/// Update (see PointerSource): the main window's cursor and the mouse's button, unless the game is shown elsewhere (the
	/// editor's game viewport) which then gives it (SetPointer). UI2 knows neither the game's view nor its cameras: each canvas
	/// taking the pointer says how a view position maps into it (PointerMapping, e.g. through the camera for a canvas placed
	/// in the world).
	class HOD_UI2_API CanvasManager final
	{
		_Singleton(CanvasManager)

	public:

		// A position in the view (pixels, origin at its top-left corner, y down) to the canvas' space. false when the canvas
		// can't take the pointer (not displayed in that view...).
		using PointerMapping = std::function<bool(const Vector2& viewPosition, const Vector2& viewSize, Vector2& outCanvasPosition)>;

		enum class PointerSource : uint8_t
		{
			Window, // The main window's cursor, pressed by the mouse's left button (the game drawn in the main window)
			Manual, // Given each frame through SetPointer (the game drawn elsewhere: the editor's game viewport, tests)
		};

	public:

		void	AddCanvas(Canvas* canvas);
		void	RemoveCanvas(Canvas* canvas);

		// canvas (added) takes the pointer through mapping, before the canvases of a lower priority: the first one whose nodes
		// catch the pointer (see Canvas::UpdatePointer) takes it, the others lose it. A null mapping takes the pointer away.
		void	SetPointerMapping(Canvas* canvas, PointerMapping mapping, int64_t priority);

		// Window by default
		PointerSource	GetPointerSource() const;
		void			SetPointerSource(PointerSource pointerSource);

		// Manual source: the pointer over the view this frame (position: see PointerMapping), with its primary button's state.
		// Without a call in a frame, there is no pointer (out of the view, or not playing). Ignored with the Window source.
		void	SetPointer(const Vector2& viewPosition, const Vector2& viewSize, bool pressed);

		// Whether the pointer was over a canvas' node catching it, or pressing one of them, at the last Update(): the input is
		// the UI's, the game shouldn't act on what is under it.
		bool	IsPointerOverUI() const;

		// Advances (see Canvas::Update) then lays out every canvas, once per frame, then gives them the pointer (read from the
		// window with the Window source).
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
		void	ReadWindowPointer();
		void	UpdatePointer();

	private:

		Vector<Entry>	_entries; // By decreasing priority

		PointerSource	_pointerSource = PointerSource::Window;
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
