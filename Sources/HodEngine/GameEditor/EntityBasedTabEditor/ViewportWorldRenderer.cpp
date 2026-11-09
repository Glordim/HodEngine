#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWorldRenderer.hpp"

namespace hod::inline editor
{
	namespace
	{
		ViewportWorldRenderer::DrawFunction            g_drawFunction = nullptr;
		ViewportWorldRenderer::SetupPlayCameraFunction g_setupPlayCameraFunction = nullptr;
	}

	/// @brief
	/// @param draw
	/// @param setupPlayCamera
	void ViewportWorldRenderer::Set(DrawFunction draw, SetupPlayCameraFunction setupPlayCamera)
	{
		g_drawFunction = draw;
		g_setupPlayCameraFunction = setupPlayCamera;
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

	/// @brief
	/// @param renderView
	/// @param world
	void ViewportWorldRenderer::SetupPlayCamera(RenderView& renderView, game::World& world)
	{
		if (g_setupPlayCameraFunction != nullptr)
		{
			g_setupPlayCameraFunction(renderView, world);
		}
	}
}
