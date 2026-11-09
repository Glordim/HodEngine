#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWorldRenderer.hpp"

namespace hod::inline editor
{
	namespace
	{
		ViewportWorldRenderer::DrawFunction g_drawFunction = nullptr;
	}

	/// @brief
	/// @param draw
	void ViewportWorldRenderer::Set(DrawFunction draw)
	{
		g_drawFunction = draw;
	}

	/// @brief
	/// @param renderView
	/// @param world
	void ViewportWorldRenderer::Draw(RenderView& renderView, game::World& world)
	{
		if (g_drawFunction != nullptr)
		{
			g_drawFunction(renderView, world);
		}
	}
}
