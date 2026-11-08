#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline editor
{
	/// @brief Playing, the mouse over a viewport's game image, given to the modules that want it as the game window's would be (UI2's
	/// canvases, see UI2Editor): the game is drawn in the viewport, not in a window of its own
	class HOD_GAME_EDITOR_API ViewportGamePointer
	{
	public:
		/// @param position In the image, in pixels from its top left corner
		/// @param resolution Of the image
		/// @param pressed The left button, pressed over the image (kept while held, even outside)
		/// @param scroll The wheel over the image (x towards the right, y up)
		using Listener = void (*)(const Vector2& position, const Vector2& resolution, bool pressed, const Vector2& scroll);

		/// @return false (with an error) if registered already
		static bool Register(Listener listener);
		static void Unregister(Listener listener);

		/// @brief By the viewports, each frame the pointer is over their image or pressed there
		static void Dispatch(const Vector2& position, const Vector2& resolution, bool pressed, const Vector2& scroll);
	};
}
