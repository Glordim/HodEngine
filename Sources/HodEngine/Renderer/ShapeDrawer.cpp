#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/ShapeDrawer.hpp"

#include "HodEngine/Renderer/BuiltinMaterialManager.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"
#include "HodEngine/Renderer/RenderView.hpp"
#include "HodEngine/Renderer/ShapeList.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cmath>
#include <limits>
#include <numbers>

namespace hod::inline renderer
{
	namespace
	{
		constexpr float Pi = std::numbers::pi_v<float>;

		const MaterialInstance* GetLineMaterialInstance()
		{
			return BuiltinMaterialManager::GetInstance()->GetBuiltinMaterialDefaultInstance(BuiltinMaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_Line);
		}

		const MaterialInstance* GetFillMaterialInstance()
		{
			return BuiltinMaterialManager::GetInstance()->GetBuiltinMaterialDefaultInstance(BuiltinMaterialManager::BuiltinMaterial::P2fC4f_Unlit_Fill_Triangle);
		}

		/// @brief Indexed vertices of one topology (line list or triangle list), pushed as one RenderCommandMesh
		struct Batch
		{
			static constexpr uint32_t MaxVertexCount = std::numeric_limits<uint16_t>::max() + 1u; // 16 bits indices

			Vector<Vector2>  _positions;
			Vector<Color>    _colors;
			Vector<uint16_t> _indices;

			uint32_t GetVertexCount() const
			{
				return (uint32_t)_positions.Size();
			}

			bool CanAdd(uint32_t vertexCount) const
			{
				return GetVertexCount() + vertexCount <= MaxVertexCount;
			}

			bool Empty() const
			{
				return _positions.Empty();
			}

			void Clear()
			{
				_positions.Clear();
				_colors.Clear();
				_indices.Clear();
			}

			/// @brief Segments between consecutive points
			void AddPolyline(const Vector2* points, uint32_t pointCount, bool closed, const Color& color)
			{
				if (pointCount < 2)
				{
					return;
				}
				uint32_t first = AddVertices(points, pointCount, color);
				uint32_t segmentCount = closed ? pointCount : pointCount - 1;
				for (uint32_t index = 0; index < segmentCount; ++index)
				{
					_indices.PushBack(static_cast<uint16_t>(first + index));
					_indices.PushBack(static_cast<uint16_t>(first + (index + 1) % pointCount));
				}
			}

			/// @brief Segments points[0]-points[1], points[2]-points[3]...
			void AddSegments(const Vector2* points, uint32_t pointCount, const Color& color)
			{
				pointCount -= pointCount % 2;
				uint32_t first = AddVertices(points, pointCount, color);
				for (uint32_t index = 0; index < pointCount; ++index)
				{
					_indices.PushBack(static_cast<uint16_t>(first + index));
				}
			}

			/// @brief Convex polygon, as a fan from its first point
			void AddFan(const Vector2* points, uint32_t pointCount, const Color& color)
			{
				if (pointCount < 3)
				{
					return;
				}
				uint32_t first = AddVertices(points, pointCount, color);
				for (uint32_t index = 1; index + 1 < pointCount; ++index)
				{
					_indices.PushBack(static_cast<uint16_t>(first));
					_indices.PushBack(static_cast<uint16_t>(first + index));
					_indices.PushBack(static_cast<uint16_t>(first + index + 1));
				}
			}

			/// @brief indices relative to vertices, a triangle list
			void AddTriangles(const Vector2* vertices, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const Color& color)
			{
				uint32_t first = AddVertices(vertices, vertexCount, color);
				for (uint32_t index = 0; index < indexCount; ++index)
				{
					_indices.PushBack(static_cast<uint16_t>(first + indices[index]));
				}
			}

			uint32_t AddVertices(const Vector2* points, uint32_t pointCount, const Color& color)
			{
				uint32_t first = GetVertexCount();
				for (uint32_t index = 0; index < pointCount; ++index)
				{
					_positions.PushBack(points[index]);
					_colors.PushBack(color);
				}
				return first;
			}

			void Push(RenderView& renderView, const MaterialInstance* materialInstance, const Matrix4& matrix, uint32_t order, uint32_t pickingId = 0) const
			{
				if (materialInstance == nullptr || Empty() || _indices.Empty())
				{
					return;
				}
				renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(_positions.Data(), nullptr, _colors.Data(), GetVertexCount(), _indices.Data(),
				                                                                                      (uint32_t)_indices.Size(), matrix, materialInstance, order, pickingId,
				                                                                                      true));
			}
		};

		/// @brief segmentCount points on the circle, counter-clockwise from startAngle
		void AppendCirclePoints(Vector<Vector2>& points, const Vector2& center, float radius, uint32_t segmentCount, float startAngle = 0.0f, float angleRange = 2.0f * Pi,
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

		/// @brief The outline of the capsule whose half circles are centered on start and end, counter-clockwise
		void AppendCapsulePoints(Vector<Vector2>& points, const Vector2& start, const Vector2& end, float radius, uint32_t segmentCount)
		{
			Vector2 axis = end - start;
			float   axisAngle = (axis.GetX() != 0.0f || axis.GetY() != 0.0f) ? std::atan2(axis.GetY(), axis.GetX()) : Pi * 0.5f;

			uint32_t halfSegmentCount = Max(segmentCount / 2, 2u);
			AppendCirclePoints(points, end, radius, halfSegmentCount, axisAngle - Pi * 0.5f, Pi, true);   // around end
			AppendCirclePoints(points, start, radius, halfSegmentCount, axisAngle + Pi * 0.5f, Pi, true); // around start
		}

		/// @brief The square of a point, its side in world units
		StaticArray<Vector2, 4> GetSquarePoints(const Vector2& center, float side)
		{
			float half = side * 0.5f;
			return {
				center + Vector2(-half, half),
				center + Vector2(half, half),
				center + Vector2(half, -half),
				center + Vector2(-half, -half),
			};
		}

		StaticArray<Vector2, 4> GetRectPoints(const Vector2& center, const Vector2& size)
		{
			Vector2 halfSize = size * 0.5f;
			return {
				center + Vector2(-halfSize.GetX(), halfSize.GetY()),
				center + Vector2(halfSize.GetX(), halfSize.GetY()),
				center + Vector2(halfSize.GetX(), -halfSize.GetY()),
				center + Vector2(-halfSize.GetX(), -halfSize.GetY()),
			};
		}

		/// @brief One shape: an outline (line list) or a fill (triangle list), in a batch of its own
		void PushOutline(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, bool closed, const Color& color, uint32_t order)
		{
			Batch batch;
			batch.AddPolyline(points, pointCount, closed, color);
			batch.Push(renderView, GetLineMaterialInstance(), matrix, order);
		}

		void PushFill(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order, uint32_t pickingId)
		{
			Batch batch;
			batch.AddFan(points, pointCount, color);
			batch.Push(renderView, GetFillMaterialInstance(), matrix, order, pickingId);
		}
	}

	/// @brief
	void ShapeDrawer::Line(RenderView& renderView, const Matrix4& matrix, const Vector2& start, const Vector2& end, const Color& color, uint32_t order)
	{
		StaticArray<Vector2, 2> points = {start, end};
		PushOutline(renderView, matrix, points.Data(), (uint32_t)points.Size(), false, color, order);
	}

	/// @brief
	void ShapeDrawer::Lines(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order)
	{
		Batch batch;
		batch.AddSegments(points, pointCount, color);
		batch.Push(renderView, GetLineMaterialInstance(), matrix, order);
	}

	/// @brief
	void ShapeDrawer::Polyline(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, bool closed, const Color& color, uint32_t order)
	{
		PushOutline(renderView, matrix, points, pointCount, closed, color, order);
	}

	/// @brief
	void ShapeDrawer::Rect(RenderView& renderView, const Matrix4& matrix, const Vector2& center, const Vector2& size, const Color& color, uint32_t order)
	{
		StaticArray<Vector2, 4> points = GetRectPoints(center, size);
		PushOutline(renderView, matrix, points.Data(), (uint32_t)points.Size(), true, color, order);
	}

	/// @brief
	void ShapeDrawer::Circle(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float radius, const Color& color, uint32_t order, uint32_t segmentCount)
	{
		Vector<Vector2> points;
		AppendCirclePoints(points, center, radius, Max(segmentCount, 3u));
		PushOutline(renderView, matrix, points.Data(), (uint32_t)points.Size(), true, color, order);
	}

	/// @brief
	void ShapeDrawer::Capsule(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float height, float radius, const Color& color, uint32_t order,
	                          uint32_t segmentCount)
	{
		float           halfStraight = Max(height * 0.5f - radius, 0.0f);
		Vector<Vector2> points;
		AppendCapsulePoints(points, center - Vector2(0.0f, halfStraight), center + Vector2(0.0f, halfStraight), radius, segmentCount);
		PushOutline(renderView, matrix, points.Data(), (uint32_t)points.Size(), true, color, order);
	}

	/// @brief
	void ShapeDrawer::FilledRect(RenderView& renderView, const Matrix4& matrix, const Vector2& center, const Vector2& size, const Color& color, uint32_t order,
	                             uint32_t pickingId)
	{
		StaticArray<Vector2, 4> points = GetRectPoints(center, size);
		PushFill(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledCircle(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float radius, const Color& color, uint32_t order, uint32_t pickingId,
	                               uint32_t segmentCount)
	{
		Vector<Vector2> points;
		AppendCirclePoints(points, center, radius, Max(segmentCount, 3u));
		PushFill(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledCapsule(RenderView& renderView, const Matrix4& matrix, const Vector2& center, float height, float radius, const Color& color, uint32_t order,
	                                uint32_t pickingId, uint32_t segmentCount)
	{
		float           halfStraight = Max(height * 0.5f - radius, 0.0f);
		Vector<Vector2> points;
		AppendCapsulePoints(points, center - Vector2(0.0f, halfStraight), center + Vector2(0.0f, halfStraight), radius, segmentCount);
		PushFill(renderView, matrix, points.Data(), (uint32_t)points.Size(), color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledPolygon(RenderView& renderView, const Matrix4& matrix, const Vector2* points, uint32_t pointCount, const Color& color, uint32_t order,
	                                uint32_t pickingId)
	{
		PushFill(renderView, matrix, points, pointCount, color, order, pickingId);
	}

	/// @brief
	void ShapeDrawer::FilledTriangles(RenderView& renderView, const Matrix4& matrix, const Vector2* vertices, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount,
	                                  const Color& color, uint32_t order, uint32_t pickingId)
	{
		const MaterialInstance* materialInstance = GetFillMaterialInstance();
		if (materialInstance == nullptr || vertices == nullptr || vertexCount == 0)
		{
			return;
		}

		Vector<Color> colors;
		colors.Resize(vertexCount, color);
		renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(vertices, nullptr, colors.Data(), vertexCount, indices,
		                                                                                      indices != nullptr ? indexCount : 0, matrix, materialInstance, order, pickingId,
		                                                                                      true));
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
		Batch batch;
		batch.AddTriangles(vertices.Data(), (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), color);
		batch.Push(renderView, GetFillMaterialInstance(), matrix, order);
	}

	/// @brief
	void ShapeDrawer::Point(RenderView& renderView, const Vector2& position, float pixelSize, const Color& color, uint32_t order)
	{
		float worldSize = GetPixelWorldSize(renderView) * Max(pixelSize, 1.0f);
		if (worldSize <= 0.0f)
		{
			return;
		}
		StaticArray<Vector2, 4> points = GetSquarePoints(position, worldSize);
		PushFill(renderView, Matrix4::Identity, points.Data(), (uint32_t)points.Size(), color, order, 0);
	}

	/// @brief
	void ShapeDrawer::Draw(RenderView& renderView, const ShapeList& shapes, uint32_t order)
	{
		const MaterialInstance* lineMaterialInstance = GetLineMaterialInstance();
		const MaterialInstance* fillMaterialInstance = GetFillMaterialInstance();

		Batch fills;
		Batch outlines;
		auto  flush = [&]()
		{
			// Fills first: an outline drawn over its fill (the order of the shapes between the two topologies is lost)
			fills.Push(renderView, fillMaterialInstance, Matrix4::Identity, order);
			outlines.Push(renderView, lineMaterialInstance, Matrix4::Identity, order);
			fills.Clear();
			outlines.Clear();
		};

		const float     pixelWorldSize = GetPixelWorldSize(renderView);
		const Vector2*  allPoints = shapes.GetPoints().Data();
		Vector<Vector2> points; // of the circles and capsules

		for (const ShapeList::Shape& shape : shapes.GetShapes())
		{
			const Vector2* shapePoints = allPoints + shape._firstPoint;
			uint32_t       pointCount = shape._pointCount;
			bool           filled = false;
			bool           closed = false;

			switch (shape._type)
			{
				case ShapeList::Type::Polyline: break;
				case ShapeList::Type::ClosedPolyline: closed = true; break;
				case ShapeList::Type::FilledPolygon: filled = true; break;
				case ShapeList::Type::Circle:
				case ShapeList::Type::FilledCircle:
					points.Clear();
					AppendCirclePoints(points, shapePoints[0], shape._size, DefaultCircleSegmentCount);
					shapePoints = points.Data();
					pointCount = (uint32_t)points.Size();
					filled = (shape._type == ShapeList::Type::FilledCircle);
					closed = true;
					break;
				case ShapeList::Type::Capsule:
				case ShapeList::Type::FilledCapsule:
					points.Clear();
					AppendCapsulePoints(points, shapePoints[0], shapePoints[1], shape._size, DefaultCircleSegmentCount);
					shapePoints = points.Data();
					pointCount = (uint32_t)points.Size();
					filled = (shape._type == ShapeList::Type::FilledCapsule);
					closed = true;
					break;
				case ShapeList::Type::Point:
				{
					if (pixelWorldSize <= 0.0f)
					{
						continue;
					}
					points.Clear();
					for (const Vector2& corner : GetSquarePoints(shapePoints[0], pixelWorldSize * Max(shape._size, 1.0f)))
					{
						points.PushBack(corner);
					}
					shapePoints = points.Data();
					pointCount = (uint32_t)points.Size();
					filled = true;
				}
				break;
			}

			if (pointCount > Batch::MaxVertexCount)
			{
				continue; // can't be indexed
			}

			Batch& batch = filled ? fills : outlines;
			if (batch.CanAdd(pointCount) == false)
			{
				flush();
			}

			if (filled)
			{
				batch.AddFan(shapePoints, pointCount, shape._color);
			}
			else
			{
				batch.AddPolyline(shapePoints, pointCount, closed, shape._color);
			}
		}
		flush();
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
