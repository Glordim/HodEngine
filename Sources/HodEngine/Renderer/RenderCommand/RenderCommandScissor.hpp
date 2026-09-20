#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Math/Matrix4.hpp"
#include "HodEngine/Math/Rect.hpp"
#include "HodEngine/Math/Vector2.hpp"

namespace hod::inline renderer
{
	/// @brief Restricts the drawing that follows (in rendering order) to the intersection of a set
	/// of clip regions, or to the whole viewport when there is none. Each region is a rect centered
	/// on its model matrix' origin; it is projected with the same matrices the meshes use, then
	/// reduced to its axis-aligned bounding box in pixels (a rotated region is over-approximated).
	///
	/// The projection is done at execution time (the view/projection matrices are only known by
	/// the command buffer then), which is why the whole stack of regions travels with the command
	/// instead of being resolved by the pusher: entering and leaving a nested clip are both a
	/// self-contained "set scissor to that stack" command.
	class HOD_RENDERER_API RenderCommandScissor : public RenderCommand
	{
	public:
		struct Region
		{
			Matrix4 _modelMatrix;
			Vector2 _size;
		};

	public:
		/// @param regions empty to remove any clipping
		/// @param viewport viewport (in pixels) the regions are projected into, i.e. RenderView::GetViewport()
		RenderCommandScissor(const Vector<Region>& regions, const Rect& viewport, uint32_t order);
		RenderCommandScissor(const RenderCommandScissor&) = delete;
		RenderCommandScissor(RenderCommandScissor&&) = delete;
		~RenderCommandScissor() override = default;

		void operator=(const RenderCommandScissor&) = delete;
		void operator=(RenderCommandScissor&&) = delete;

	public:
		void     Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial = nullptr) override;
		uint32_t GetRenderingOrder() const override;

	private:
		Vector<Region> _regions;
		Rect           _viewport;
		uint32_t       _order;
	};
}
