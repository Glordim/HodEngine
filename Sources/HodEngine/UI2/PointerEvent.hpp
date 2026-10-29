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
			Enter,   // The pointer came over the node (or one of its descendants not handling it themselves)
			Exit,    // It left it
			Press,   // The primary button went down over it. Bubbles from the hit node up to the first Interaction handling it
			Release, // The button went up, wherever the pointer is: only sent to the node that handled the Press
			Click,   // Right after Release, when the pointer is still over the node that handled the Press
		};

		Type	_type = Type::Enter;
		Vector2	_position = Vector2::Zero; // In canvas units
		Canvas*	_canvas = nullptr;         // The canvas dispatching it, e.g. to emit an action (see Canvas::EmitAction)
	};
}
