#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/ShapeDrawer.hpp"

#include "HodEngine/Renderer/BuiltinMaterialManager.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"
#include "HodEngine/Renderer/RenderView.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cmath>
#include <numbers>

namespace hod::inline renderer
{
	namespace
	{
		const MaterialInstance* GetLineMaterialInstance()
		{
			return BuiltinMaterialManager::GetInstance()->GetBuiltinMaterialDefaultInstance(BuiltinMaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_Line);
		}

		const MaterialInstance* GetFillMaterialInstance()
		{
			return BuiltinMaterialManager::GetInstance()->GetBuiltinMaterialDefaultInstance(BuiltinMaterialManager::BuiltinMaterial::P2fC4f_Unlit_Fill_Triangle);
		}

		void Push(RenderView& renderView, const MaterialInstance* materialInstance, const Matrix4& matrix, const Vector2* positions, uint32_t vertexCount,
		          const uint16_t* indices, uint32_t indexCount, const Color& color, uint32_t order, uint32_t pickingId)
		{
			if (materialInstance == nullptr || positions == nullptr || vertexCount == 0)
			{
				return;
			}

			Vector<Color> colors;
			colors.Resize(vertexCount, color);

			renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(positions, nullptr, colors.Data(), vertexCount, indices, indexCount, matrix,
			                                                                                      materialInstance, order, pickingId, true));
		}

		/// @brief segmentCount points on the circle, counter-clockwise from angle 0
		void AppendCirclePoints(Vector<Vector2>& points, const Vector2& center, float radius, uint32_t segmentCount, float startAngle = 0.0f, float angleRange = 2.0f * std::numbers::pi_v<float>,
		                        bool includeEnd = false)
		{
			uint32_t pointCount = includeEnd ? segmentCount + 1 : segmentCount;
			float    angleStep = angleRange / static_cast<float>(segmentCount);
			for (uint32_t index = 0; index < pointCount; ++index)
			{
				float angle = startAngle + angleStep * static_cast<float>(index);
				points.PushBack(Vector2(center.GetX() + radius * std::cos(angle), center.GetY() + radius * std::sin(angle)));
			}
		}

		void PushClosedPolyline(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order)
		{
			ShapeDrawer::Polyline(renderView, matrix, points, pointCount, true, color, order);
		}
	}

	/// @brief
	void ShapeDrawer::Line(RenderView& renderView, const Matrix4& matrix, const Vector2& start, const Vector2& end, const Color& color, uint32_t order)
	{
		StaticArray<Vector2, 2> points = {start, end};
		Push(renderView, GetLineMaterialInstance(), matrix, points.Data(), (uint32_t)points.Size(), nullptr, 0, color, order, 0);
	}

	/// @brief
	void ShapeDrawer::Lines(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order)
	{
		pointCount -= pointCount % 2;
		Push(renderView, GetLineMaterialInstance(), matrix, points, pointCount, nullptr, 0, color, order, 0);
	}

	/// @brief
	void ShapeDrawer::Polyline(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, bool closed, const Color& color, uint32_t order)
	{
		if (pointCount < 2)
		{
			return;
		}

		uint32_t         segmentCount = closed ? pointCount : pointCount - 1;
		Vector<uint16_t> indices;
		indices.Reserve(segmentCount * 2);
		for (uint32_t index = 0; index < segmentCount; ++index)
		{
			indices.PushBack(static_cast<uint16_t>(index));
			indices.PushBack(static_cast<uint16_t>((index + 1) % pointCount));
		}
		Push(renderView, GetLineMaterialInstance(), matrix, points, pointCount, indices.Data(), (uint32_t)indices.Size(), color, order, 0);
	}

	/// @brief
	void ShapeDrawer::Rect(RenderView& renderView, const Matrix4& matrix, const Vector2& center, const Vector2& size, const Color& color, uint32_t order)
	{
		Vector2                 halfSize = size * 0.5f;
		StaticArray<Vector2, 4> points = {
			center + Vector2(-halfSize.GetX(), halfSize.GetY()),
			center + Vector2(halfSize.GetX(), halfSize.GetY()),
			center + Vector2(halfSize.GetX(), -halfSize.GetY()),
			center + Vector2(-halfSize.GetX(), -halfSize.GetY()),
		};
		PushClosedPolyline(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order);
	}

	/// @brief
	void ShapeDrawer::Circle(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float radius, const Color& color, uint32_t order, uint32_t segmentCount)
	{
		Vector<Vector2> points;
		points.Reserve(segmentCount);
		AppendCirclePoints(points, center, radius, Max(segmentCount, 3u));
		PushClosedPolyline(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order);
	}

	/// @brief
	void ShapeDrawer::Capsule(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float height, float radius, const Color& color, uint32_t order,
	                          uint32_t segmentCount)
	{
		constexpr float pi = std::numbers::pi_v<float>;

		uint32_t halfSegmentCount = Max(segmentCount / 2, 2u);
		float    halfStraight = Max(height * 0.5f - radius, 0.0f);

		Vector<Vector2> points;
		points.Reserve((halfSegmentCount + 1) * 2);
		AppendCirclePoints(points, center + Vector2(0.0f, halfStraight), radius, halfSegmentCount, 0.0f, pi, true);  // top half circle, right to left
		AppendCirclePoints(points, center - Vector2(0.0f, halfStraight), radius, halfSegmentCount, pi, pi, true);    // bottom half circle, left to right
		PushClosedPolyline(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order);
	}

	/// @brief
	void ShapeDrawer::FilledRect(RenderView& renderView, const Matrix4& matrix, const Vector2& center, const Vector2& size, const Color& color, uint32_t order,
	                             uint32_t pickingId)
	{
		Vector2                 halfSize = size * 0.5f;
		StaticArray<Vector2, 4> vertices = {
			center + Vector2(-halfSize.GetX(), halfSize.GetY()),
			center + Vector2(halfSize.GetX(), halfSize.GetY()),
			center + Vector2(halfSize.GetX(), -halfSize.GetY()),
			center + Vector2(-halfSize.GetX(), -halfSize.GetY()),
		};
		static const StaticArray<uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};
		Push(renderView, GetFillMaterialInstance(), matrix, vertices.Data(), (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledCircle(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float radius, const Color& color, uint32_t order, uint32_t pickingId,
	                               uint32_t segmentCount)
	{
		Vector<Vector2> points;
		points.Reserve(segmentCount);
		AppendCirclePoints(points, center, radius, Max(segmentCount, 3u));
		FilledPolygon(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledPolygon(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order,
	                                uint32_t pickingId)
	{
		if (pointCount < 3)
		{
			return;
		}

		Vector<uint16_t> indices;
		indices.Reserve((pointCount - 2) * 3);
		for (uint32_t index = 1; index + 1 < pointCount; ++index)
		{
			indices.PushBack(0);
			indices.PushBack(static_cast<uint16_t>(index));
			indices.PushBack(static_cast<uint16_t>(index + 1));
		}
		Push(renderView, GetFillMaterialInstance(), matrix, points, pointCount, indices.Data(), (uint32_t)indices.Size(), color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledTriangles(RenderView& renderView, const Matrix4& matrix, const Vector2* vertices, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount,
	                                  const Color& color, uint32_t order, uint32_t pickingId)
	{
		Push(renderView, GetFillMaterialInstance(), matrix, vertices, vertexCount, indices, indices != nullptr ? indexCount : 0, color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledFrame(RenderView& renderView, const Matrix4& matrix, const Vector2& outerPosition, const Vector2& outerSize, const Vector2& innerPosition,
	                              const Vector2& innerSize, const Color& color, uint32_t order)
	{
		Vector2 outerMax = outerPosition + outerSize;
		Vector2 innerMax = innerPosition + innerSize;

		// Counter-clockwise from the bottom-left corner: outer ring then inner ring
		StaticArray<Vector2, 8> vertices = {
			outerPosition,
			Vector2(outerMax.GetX(), outerPosition.GetY()),
			outerMax,
			Vector2(outerPosition.GetX(), outerMax.GetY()),
			innerPosition,
			Vector2(innerMax.GetX(), innerPosition.GetY()),
			innerMax,
			Vector2(innerPosition.GetX(), innerMax.GetY()),
		};
		// One quad per side (bottom, right, top, left), between corner i and i + 1 of each ring
		static const StaticArray<uint16_t, 24> indices = {
			0, 1, 5, 0, 5, 4, 1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 0, 4, 3, 4, 7,
		};
		Push(renderView, GetFillMaterialInstance(), matrix, vertices.Data(), (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), color, order, 0);
	}

	/// @brief
	void ShapeDrawer::Point(RenderView& renderView, const Vector2& position, float pixelSize, const Color& color, uint32_t order)
	{
		float worldSize = GetPixelWorldSize(renderView) * Max(pixelSize, 1.0f);
		if (worldSize <= 0.0f)
		{
			return;
		}
		FilledRect(renderView, Matrix4::Identity, position, Vector2(worldSize, worldSize), color, order);
	}

	/// @brief
	float ShapeDrawer::GetPixelWorldSize(const RenderView& renderView)
	{
		float projectionScale = std::abs(renderView.GetProjectionMatrix()[0][0]);
		float resolutionX = renderView.GetRenderResolution().GetX();
		if (projectionScale <= 0.0f || resolutionX <= 0.0f)
		{
			return 0.0f;
		}
		return 2.0f / (projectionScale * resolutionX); // the projection maps [-1, 1] (2 units of clip space) to the width
	}
}
