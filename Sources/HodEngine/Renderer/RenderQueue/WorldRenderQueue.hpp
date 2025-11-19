#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RenderQueue/RenderQueue.hpp"

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API WorldRenderQueue : public RenderQueue
	{
	public:
					WorldRenderQueue() = default;

		bool		Prepare(const RenderView& renderView) override;
	};
}
