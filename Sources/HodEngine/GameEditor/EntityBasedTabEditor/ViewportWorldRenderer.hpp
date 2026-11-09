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
		/// @brief The world, in renderView whose camera is set up already (the viewport's, or the game's when playing, see SetupPlayCameraFunction)
		using DrawFunction = void (*)(RenderView& renderView, game::World& world);

		/// @brief The game's camera of the world, in renderView: by the viewports when playing, whatever draws the world (its render, a debug view...)
		using SetupPlayCameraFunction = void (*)(RenderView& renderView, game::World& world);

		/// @param draw nullptr: none (the module unloading)
		/// @param setupPlayCamera nullptr: none, the viewports keep their default camera when playing
		static void Set(DrawFunction draw, SetupPlayCameraFunction setupPlayCamera);

		/// @brief By the viewports
		static void Draw(RenderView& renderView, game::World& world);

		/// @brief By the viewports, when playing
		static void SetupPlayCamera(RenderView& renderView, game::World& world);
	};
}
