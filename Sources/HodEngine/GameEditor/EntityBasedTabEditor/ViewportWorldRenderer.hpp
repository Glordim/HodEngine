#pragma once
#include "HodEngine/GameEditor/Export.hpp"

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline game
{
	class World;
}

namespace hod::inline editor
{
	/// @brief How the viewports draw their world (its render, when no ViewportDebugView replaces it), given by the module that renders the
	/// worlds (RendererEditor): GameEditor doesn't know it. Without it, the viewports only show the gizmos
	class HOD_GAME_EDITOR_API ViewportWorldRenderer
	{
	public:
		/// @brief The world, in renderView whose camera is set up already (the viewport's, or none when playing)
		using DrawFunction = void (*)(RenderView& renderView, game::World& world);

		/// @param draw nullptr: none (the module unloading)
		static void Set(DrawFunction draw);

		/// @brief By the viewports
		static void Draw(RenderView& renderView, game::World& world);
	};
}
