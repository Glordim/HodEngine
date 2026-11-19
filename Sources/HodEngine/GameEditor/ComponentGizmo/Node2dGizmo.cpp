#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/GameEditor/ComponentGizmo/Node2dGizmo.hpp"
#include <HodEngine/Game/Components/Node2dComponent.hpp>

#include <HodEngine/Editor/Gizmos/Handles.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include "HodEngine/Core/StaticArray.hpp"

#undef max

namespace hod::inline editor
{
	/// @brief
	/// @param node2d
	/// @param viewport
	/// @param active
	/// @return
	bool Node2dGizmo::OnDraw(Node2dComponent& node2d, GizmoViewport& viewport, bool active)
	{
		if (active == false)
		{
			return false;
		}

		float           handleSize = Handles::GetHandleSize(0.002f, viewport);
		constexpr float thickness = 0.05f;
		constexpr float length = 1.0f;
		constexpr float headHeight = 0.15f;
		constexpr float headLength = 0.25f;
		constexpr float lineLength = length - headLength;
		constexpr float squareSize = 0.25f;
		constexpr float squareOffset = 0.5f;

		StaticArray<Vector2, 12> verticesX = {
			Vector2(-thickness * 0.5f, -thickness * 0.5f),
			Vector2(thickness * 0.5f, thickness * 0.5f),
			Vector2(thickness * 0.5f, -thickness * 0.5f),

			Vector2(thickness * 0.5f, thickness * 0.5f),
			Vector2(thickness * 0.5f, -thickness * 0.5f),
			Vector2(lineLength, thickness * 0.5f),

			Vector2(0.0f, -thickness * 0.5f),
			Vector2(lineLength, -thickness * 0.5f),
			Vector2(lineLength, thickness * 0.5f),

			Vector2(lineLength, headHeight * 0.5f),
			Vector2(lineLength, -headHeight * 0.5f),
			Vector2(length, 0.0f),
		};

		StaticArray<Vector2, 12> verticesY = {
			Vector2(-thickness * 0.5f, -thickness * 0.5f),
			Vector2(-thickness * 0.5f, thickness * 0.5f),
			Vector2(thickness * 0.5f, thickness * 0.5f),

			Vector2(thickness * 0.5f, thickness * 0.5f),
			Vector2(-thickness * 0.5f, thickness * 0.5f),
			Vector2(thickness * 0.5f, lineLength),

			Vector2(-thickness * 0.5f, 0.0f),
			Vector2(-thickness * 0.5f, lineLength),
			Vector2(thickness * 0.5f, lineLength),

			Vector2(headHeight * 0.5f, lineLength),
			Vector2(-headHeight * 0.5f, lineLength),
			Vector2(0.0f, length),
		};

		constexpr float squareMin = squareOffset - squareSize * 0.5f;
		constexpr float squareMax = squareOffset + squareSize * 0.5f;
		StaticArray<Vector2, 6> verticesSquare = {
			Vector2(squareMin, squareMin), Vector2(squareMax, squareMin), Vector2(squareMax, squareMax),
			Vector2(squareMin, squareMin), Vector2(squareMax, squareMax), Vector2(squareMin, squareMax),
		};

		// The node's position is in its parent's space; the handles are shown on the node, rotated with it, at a constant size on screen
		Matrix4 worldMatrix = node2d.GetWorldMatrix();
		Matrix4 parentMatrix = worldMatrix * Matrix4::Inverse(node2d.GetLocalMatrix());

		Matrix4 drawMatrix = worldMatrix;
		drawMatrix.RemoveScale();
		Vector4 xAxis = drawMatrix * Vector4(1.0f, 0.0f, 0.0f, 0.0f);
		Vector4 yAxis = drawMatrix * Vector4(0.0f, 1.0f, 0.0f, 0.0f);
		drawMatrix *= Matrix4::Scale(Vector2(handleSize, handleSize));

		Vector2 position = node2d.GetPosition();
		bool    changed = false;
		changed |= Handles::FreeMoveMesh(viewport, HandleId(&node2d, 0), parentMatrix, position, drawMatrix, verticesSquare.Data(), verticesSquare.Size(),
		                                 Color(0.0f, 0.0f, 1.0f, 1.0f));
		changed |= Handles::FreeMoveMesh(viewport, HandleId(&node2d, 1), parentMatrix, position, drawMatrix, verticesX.Data(), verticesX.Size(), Color(1.0f, 0.0f, 0.0f, 1.0f),
		                                 Vector2(xAxis.GetX(), xAxis.GetY()));
		changed |= Handles::FreeMoveMesh(viewport, HandleId(&node2d, 2), parentMatrix, position, drawMatrix, verticesY.Data(), verticesY.Size(), Color(0.0f, 1.0f, 0.0f, 1.0f),
		                                 Vector2(yAxis.GetX(), yAxis.GetY()));
		if (changed)
		{
			node2d.SetPosition(position);
		}

		return changed;
	}
}
