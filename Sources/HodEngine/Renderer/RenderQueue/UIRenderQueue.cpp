#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RenderQueue/UIRenderQueue.hpp"
#include "HodEngine/Renderer/RenderView.hpp"

#include <algorithm>

namespace hod::inline renderer
{
	/// @brief
	/// @param renderView
	/// @return
	bool UIRenderQueue::Prepare(const RenderView& renderView)
	{
		Vector2 resolution = renderView.GetRenderResolution();
		_projection = Matrix4::OrthogonalProjection(-resolution.GetX() * 0.5f, resolution.GetX() * 0.5f, -resolution.GetY() * 0.5f, resolution.GetY() * 0.5f, -1.0f, 1.0f);
		_view = Matrix4::Identity;
		_viewport = renderView.GetViewport();

		// Stable: the commands of a same order (e.g. a UI canvas' ones, see ui2::DrawContext) are drawn in the order they were pushed
		std::stable_sort(_renderCommands.begin(), _renderCommands.end(), [](RenderCommand* a, RenderCommand* b) { return a->GetRenderingOrder() < b->GetRenderingOrder(); });

		return true;
	}
}
