#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RenderQueue/WorldRenderQueue.hpp"
#include "HodEngine/Renderer/RenderView.hpp"

#include <algorithm>

namespace hod::inline renderer
{
	/// @brief
	/// @param renderView
	/// @return
	bool WorldRenderQueue::Prepare(const RenderView& renderView)
	{
		_projection = renderView.GetProjectionMatrix();
		_view = Matrix4::Inverse(renderView.GetViewMatrix());
		_viewport = renderView.GetViewport();

		// Stable: the commands of a same order (e.g. a UI canvas' ones, see ui2::DrawContext) are drawn in the order they were pushed
		std::stable_sort(_renderCommands.begin(), _renderCommands.end(), [](RenderCommand* a, RenderCommand* b) { return a->GetRenderingOrder() < b->GetRenderingOrder(); });
		return true;
	}
}
