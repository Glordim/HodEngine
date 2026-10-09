#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandScissor.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline ui2
{
	/// @brief State shared by every Node of one Canvas while it pushes its render commands (see
	/// Node::PushRenderCommand): where the commands go (render view + queue), how canvas space maps
	/// into that queue, the painter's-order counter, and the stack of active clip rects (MaskNode).
	///
	/// Nodes stay unaware of all of that: they describe what they draw in their own local space and
	/// hand it over via PushMesh(), which is what makes a Canvas renderable both in the editor
	/// viewport (world queue) and in game (UI queue, see Canvas::ComputeFitMatrix).
	class HOD_UI2_API DrawContext
	{
	public:
		/// @param baseMatrix maps canvas space into the queue's space (identity: canvas units == queue units)
		/// @param firstOrder rendering order of the first command; every command then gets the next one, so
		/// the queue's (unstable) sort still ends up drawing in push order
		DrawContext(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix, uint32_t firstOrder = 1);
		DrawContext(const DrawContext&) = delete;
		DrawContext(DrawContext&&) = delete;
		~DrawContext() = default;

		DrawContext& operator=(const DrawContext&) = delete;
		DrawContext& operator=(DrawContext&&) = delete;

		/// @brief Queues one indexed triangle mesh. `uvs` may be null; positions/uvs/indices are copied.
		/// @param canvasMatrix the drawing node's Node::ComputeCanvasMatrix()
		/// @param materialInstance ownership is taken: destroyed once the frame using it has been rendered
		void PushMesh(const Vector2* positions, const Vector2* uvs, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const Matrix4& canvasMatrix,
		              MaterialInstance* materialInstance);

		/// @brief Clips everything queued until the matching PopClipRect() to the given rect, intersected
		/// with the clip rects already active. Must be balanced with PopClipRect().
		/// @param canvasMatrix the clipping node's Node::ComputeCanvasMatrix()
		/// @param size the rect's size, centered on the matrix origin
		void PushClipRect(const Matrix4& canvasMatrix, const Vector2& size);
		void PopClipRect();

	private:
		void PushClipCommand();

	private:
		RenderView&                 _renderView;
		RenderView::RenderQueueType _queueType;
		Matrix4                     _baseMatrix;
		uint32_t                    _nextOrder;

		Vector<RenderCommandScissor::Region> _clipRegions;
	};
}
