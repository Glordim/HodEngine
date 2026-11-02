#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline ui2
{
	class Canvas;

	/// @brief What the pointer did, as a Canvas delivers it to a node's Interaction (see Canvas::UpdatePointer).
	struct HOD_UI2_API PointerEvent
	{
		enum class Type : uint8_t
		{
			Enter,     // The pointer came over the node (or one of its descendants not handling it themselves)
			Exit,      // It left it
			Press,     // The primary button went down over it. Bubbles from the hit node up to the first Interaction handling it
			Release,   // The button went up, wherever the pointer is: only sent to the node that handled the Press. Also when a drag
			           // is taken by another node (see DragBegin), without Click then
			Click,     // Right after Release, when the pointer is still over the node that handled the Press, and nothing was dragged
			DragBegin, // The pointer moved beyond the drag threshold while pressed (right on the press for an Interaction that doesn't
			           // use one, see Interaction::UsesDragThreshold). Bubbles from the node hit by the press up to the first
			           // Interaction handling it, which takes the press over (a ScrollView from the Button pressed in it)
			Drag,      // The pointer moved, dragging: only sent to the node that handled DragBegin, with _delta
			DragEnd,   // The button went up after a drag (followed by Release), or the pointer was lost
			Scroll,    // The wheel turned over it, _scroll giving how much. Bubbles from the hit node up to the first Interaction handling it
			PressWithin, // The button went down over it or one of its descendants: sent to every Interaction from the hit node up, before
			             // the Press, whatever handles it (a ScrollView stops moving under a Button pressed in it)
		};

		Type	_type = Type::Enter;
		Vector2	_position = Vector2::Zero; // In canvas units
		Vector2	_delta = Vector2::Zero;    // Drag: the move since the previous Drag (or DragBegin), in canvas units
		Vector2	_scroll = Vector2::Zero;   // Scroll: wheel notches, y > 0 away from the user (the content above wanted)
		Canvas*	_canvas = nullptr;         // The canvas dispatching it, e.g. to emit an action (see Canvas::EmitAction)
	};
}
