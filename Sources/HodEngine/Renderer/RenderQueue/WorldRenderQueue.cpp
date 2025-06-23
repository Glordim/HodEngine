#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderQueue/WorldRenderQueue.hpp"
#include "HodEngine/Renderer/RenderView.hpp"

namespace hod::renderer
{
	/// @brief 
	/// @param context 
	/// @return 
	bool WorldRenderQueue::Prepare(const RenderView& renderView)
	{
		_projection = renderView.GetProjectionMatrix();
		_view = Matrix4::Inverse(renderView.GetViewMatrix());
		_viewport = renderView.GetViewport();

		std::sort(_renderCommands.begin(), _renderCommands.end(),
		[](RenderCommand* a, RenderCommand* b)
		{
			return a->GetRenderingOrder() < b->GetRenderingOrder();
		});
		return true;
	}
}
