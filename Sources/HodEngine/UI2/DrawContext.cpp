#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include "HodEngine/Core/Assert.hpp"

#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

namespace hod::inline ui2
{
	/// @brief
	/// @param renderView
	/// @param queueType
	/// @param baseMatrix
	/// @param firstOrder
	DrawContext::DrawContext(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix, uint32_t firstOrder)
	: _renderView(renderView)
	, _queueType(queueType)
	, _baseMatrix(baseMatrix)
	, _nextOrder(firstOrder)
	{
	}

	/// @brief
	/// @param positions
	/// @param uvs
	/// @param vertexCount
	/// @param indices
	/// @param indexCount
	/// @param canvasMatrix
	/// @param materialInstance
	void DrawContext::PushMesh(const Vector2* positions, const Vector2* uvs, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const Matrix4& canvasMatrix,
	                           MaterialInstance* materialInstance)
	{
		_renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(positions, uvs, nullptr, vertexCount, indices, indexCount,
		                                                                                     _baseMatrix * canvasMatrix, materialInstance, _nextOrder++),
		                              _queueType);
		_renderView.DeleteAfter(materialInstance);
	}

	/// @brief
	/// @param canvasMatrix
	/// @param size
	void DrawContext::PushClipRect(const Matrix4& canvasMatrix, const Vector2& size)
	{
		RenderCommandScissor::Region region;
		region._modelMatrix = _baseMatrix * canvasMatrix;
		region._size = size;
		_clipRegions.PushBack(region);

		PushClipCommand();
	}

	/// @brief
	void DrawContext::PopClipRect()
	{
		Assert(_clipRegions.Empty() == false);

		_clipRegions.PopBack();
		PushClipCommand();
	}

	/// @brief Queues the scissor command matching the current clip stack (no clip once it is empty).
	void DrawContext::PushClipCommand()
	{
		_renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandScissor>(_clipRegions, _renderView.GetViewport(), _nextOrder++), _queueType);
	}
}
