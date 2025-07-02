#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/Node2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include <HodEngine/Game/Components/Node2dComponent.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#include "HodEngine/Editor/ViewportWindow.hpp"

#undef max

namespace hod::editor
{
	/// @brief 
	Node2dComponentCustomEditor::Node2dComponentCustomEditor()
	{
		_freeMoveHandle = Gizmos::GenerateHandle();

		_XAxisHandle = Gizmos::GenerateHandle();
		_YAxisHandle = Gizmos::GenerateHandle();
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool Node2dComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		game::Node2dComponent* node2D = static_cast<game::Node2dComponent*>(component);
		if (node2D == nullptr)
		{
			return false;
		}

		bool changed = false;

		float handleSize = Gizmos::GetHandleSize(0.002f, viewport);
		constexpr float thickness = 0.05f;
		constexpr float length = 1.0f;
		constexpr float headHeight = 0.15f;
		constexpr float headLength = 0.25f;
		constexpr float lineLength = length - headLength;
		constexpr float squareSize = 0.25f;
		constexpr float squareOffset = 0.5f;

		std::array<Vector2, 12> verticesX = {
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

		std::array<Vector2, 12> verticesY = {
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

		Matrix4 worldMatrix = node2D->GetWorldMatrix();
		worldMatrix.RemoveScale();
		worldMatrix *= Matrix4::Scale(Vector2(handleSize, handleSize));

		bool interacted = false;
		interacted |= Gizmos::FreeMoveRect(_freeMoveHandle, worldMatrix, Vector2::One * squareOffset, Vector2::One * squareSize, Color(0.0f, 0.0f, 1.0f, 1.0f), Color(0.5f, 0.5f, 1.0f, 1.0f), viewport);
		interacted |= Gizmos::FreeMoveMesh(_XAxisHandle, worldMatrix, verticesX.data(), (uint32_t)verticesX.size(), Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 0.5f, 0.5f, 1.0f), viewport);
		interacted |= Gizmos::FreeMoveMesh(_YAxisHandle, worldMatrix, verticesY.data(), (uint32_t)verticesY.size(), Color(0.0f, 1.0f, 0.0f, 1.0f), Color(0.5f, 1.0f, 0.5f, 1.0f), viewport);
		if (interacted)
		{
			if (_freeMoveHandle._justPressed || _XAxisHandle._justPressed || _YAxisHandle._justPressed)
			{
				_initialPosition = node2D->GetPosition();
			}
			else if (_freeMoveHandle._canceled || _XAxisHandle._canceled || _YAxisHandle._canceled)
			{
				node2D->SetPosition(_initialPosition);
				changed = true;
			}
			else
			{
				if (_freeMoveHandle._pressed)
				{
					node2D->SetPosition(_initialPosition + _freeMoveHandle._delta);
					changed = true;
				}
				else if (_XAxisHandle._pressed)
				{
					node2D->SetPosition(Vector2(_initialPosition.GetX() + _XAxisHandle._delta.GetX(), _initialPosition.GetY()));
					changed = true;
				}
				else if (_YAxisHandle._pressed)
				{
					node2D->SetPosition(Vector2(_initialPosition.GetX(), _initialPosition.GetY() + _YAxisHandle._delta.GetY()));
					changed = true;
				}
			}
		}

		return changed;
	}
}
