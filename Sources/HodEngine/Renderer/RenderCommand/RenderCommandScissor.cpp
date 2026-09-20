#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandScissor.hpp"

#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include <HodEngine/Math/Vector4.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace hod::inline renderer
{
	/// @brief
	/// @param regions
	/// @param viewport
	/// @param order
	RenderCommandScissor::RenderCommandScissor(const Vector<Region>& regions, const Rect& viewport, uint32_t order)
	: RenderCommand()
	, _regions(regions)
	, _viewport(viewport)
	, _order(order)
	{
	}

	/// @brief
	/// @param commandBuffer
	void RenderCommandScissor::Execute(CommandBuffer* commandBuffer, MaterialInstance* /*overrideMaterial*/)
	{
		// Applied even during the picking pass (overrideMaterial != nullptr): the clip stack must stay balanced.
		float minX = _viewport._position.GetX();
		float minY = _viewport._position.GetY();
		float maxX = minX + _viewport._size.GetX();
		float maxY = minY + _viewport._size.GetY();

		Matrix4 viewProjection = commandBuffer->_projection * commandBuffer->_view;
		for (const Region& region : _regions)
		{
			Matrix4 mvp = viewProjection * region._modelMatrix;
			Vector2 halfSize = region._size * 0.5f;

			float regionMinX = std::numeric_limits<float>::max();
			float regionMinY = std::numeric_limits<float>::max();
			float regionMaxX = std::numeric_limits<float>::lowest();
			float regionMaxY = std::numeric_limits<float>::lowest();

			const std::array<Vector2, 4> corners = {
				Vector2(-halfSize.GetX(), halfSize.GetY()),
				Vector2(halfSize.GetX(), halfSize.GetY()),
				Vector2(halfSize.GetX(), -halfSize.GetY()),
				Vector2(-halfSize.GetX(), -halfSize.GetY()),
			};
			for (const Vector2& corner : corners)
			{
				Vector4 clip = mvp * Vector4(corner.GetX(), corner.GetY(), 0.0f, 1.0f);
				float   inverseW = clip.GetW() != 0.0f ? 1.0f / clip.GetW() : 1.0f;
				float   ndcX = clip.GetX() * inverseW;
				float   ndcY = clip.GetY() * inverseW;

				// Framebuffer origin is top-left, NDC +Y is up.
				float pixelX = _viewport._position.GetX() + (ndcX * 0.5f + 0.5f) * _viewport._size.GetX();
				float pixelY = _viewport._position.GetY() + (0.5f - ndcY * 0.5f) * _viewport._size.GetY();

				regionMinX = std::min(regionMinX, pixelX);
				regionMinY = std::min(regionMinY, pixelY);
				regionMaxX = std::max(regionMaxX, pixelX);
				regionMaxY = std::max(regionMaxY, pixelY);
			}

			minX = std::max(minX, std::floor(regionMinX));
			minY = std::max(minY, std::floor(regionMinY));
			maxX = std::min(maxX, std::ceil(regionMaxX));
			maxY = std::min(maxY, std::ceil(regionMaxY));
		}

		Rect scissor;
		scissor._position = Vector2(minX, minY);
		scissor._size = Vector2(std::max(maxX - minX, 0.0f), std::max(maxY - minY, 0.0f));
		commandBuffer->SetScissor(scissor);
	}

	/// @brief
	/// @return
	uint32_t RenderCommandScissor::GetRenderingOrder() const
	{
		return _order;
	}
}
