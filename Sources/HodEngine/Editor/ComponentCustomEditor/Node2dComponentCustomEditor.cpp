#include "HodEngine/Editor/Pch.hpp"
#include "Node2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/ImGui/DearImGui/ImGuizmo.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#include "HodEngine/Editor/ViewportWindow.hpp"

namespace hod::editor
{
	/// @brief 
	Node2dComponentCustomEditor::Node2dComponentCustomEditor()
	{
		_materialInstanceCenterNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceCenterNormal->SetVec4("UBO.color", Vector4(0.5f, 0.5f, 0.5f, 1.0f));

		_materialInstanceCenterHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceCenterHightlight->SetVec4("UBO.color", Vector4(1.0f, 0.5f, 0.5f, 1.0f));

		_materialInstanceAxisXNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisXNormal->SetVec4("UBO.color", Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		_materialInstanceAxisXHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisXHightlight->SetVec4("UBO.color", Vector4(1.0f, 0.5f, 0.5f, 1.0f));

		_pickingIdAxisX = renderer::PickingManager::GetInstance()->GenerateId();

		_materialInstanceAxisYNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisYNormal->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));

		_materialInstanceAxisYHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisYHightlight->SetVec4("UBO.color", Vector4(0.5f, 1.0f, 0.5f, 1.0f));

		_pickingIdAxisY = renderer::PickingManager::GetInstance()->GenerateId();

		_materialInstanceAxisZNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisZNormal->SetVec4("UBO.color", Vector4(0.0f, 0.0f, 1.0f, 1.0f));

		_materialInstanceAxisZHightlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		_materialInstanceAxisZHightlight->SetVec4("UBO.color", Vector4(0.5f, 0.5f, 1.0f, 1.0f));

		_pickingIdAxisZ = renderer::PickingManager::GetInstance()->GenerateId();
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool Node2dComponentCustomEditor::OnDrawGizmo(std::shared_ptr<game::Component> component, ViewportWindow& viewport)
	{
		bool changed = false;

		std::shared_ptr<game::Node2dComponent> node2D = std::static_pointer_cast<game::Node2dComponent>(component);
		if (node2D != nullptr)
		{
			/*
			float matrix[16];
			float position[3];
			position[0] = node2D->GetPosition().GetX();
			position[1] = node2D->GetPosition().GetY();
			position[2] = 0.0f;
			float rotation[3];
			rotation[0] = 0.0f;
			rotation[1] = 0.0f;
			rotation[2] = node2D->GetRotation();
			float scale[3];
			scale[0] = node2D->GetScale().GetX();
			scale[1] = node2D->GetScale().GetY();
			scale[2] = 1.0f;
			ImGuizmo::RecomposeMatrixFromComponents(position, rotation, scale, matrix);

			if (ImGuizmo::Manipulate((float*)&view, (float*)&projection, operation, ImGuizmo::MODE::LOCAL, matrix))
			{
				ImGuizmo::DecomposeMatrixToComponents(matrix, position, rotation, scale);
				node2D->SetPosition(Vector2(position[0], position[1]));
				node2D->SetRotation(rotation[2]);
				node2D->SetScale(Vector2(scale[0], scale[1]));
				return true;
			}
			*/

			ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
			Vector2 mousePosition(mousePos.x, mousePos.y);
			Color pickingColor = viewport.GetPickingRenderTarget()->GetColorTexture()->ReadPixel(mousePosition);
			uint32_t pickingId = renderer::PickingManager::ConvertColorToId(pickingColor);

			if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left])
			{
				if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] == 0.0f && pickingId != 0 && _movingAxis == 0)
				{
					if (pickingId == _pickingIdAxisX || pickingId == _pickingIdAxisY || pickingId == _pickingIdAxisZ)
					{
						_movingAxis = pickingId;

						Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
						_pickingOffset = mouseWorldPos - node2D->GetPosition();
					}
				}
			}
			else
			{
				_movingAxis = 0;
			}

			Matrix4 finalMatrix = Matrix4::Translation(node2D->GetWorldMatrix().GetTranslation()); // Backup gizmo matrix before modification to avoid desync/lag
			
			if (_movingAxis != 0 && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
			{
				Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
				Vector2 previousPosition = node2D->GetPosition();
				Vector2 newPosition = mouseWorldPos - _pickingOffset;
				if (_movingAxis == _pickingIdAxisX)
				{
					newPosition.SetY(previousPosition.GetY());
				}
				else if (_movingAxis == _pickingIdAxisY)
				{
					newPosition.SetX(previousPosition.GetX());
				}

				if (newPosition != previousPosition)
				{
					node2D->SetPosition(newPosition);
					changed = true;
				}				
			}

			constexpr float thickness = 0.05f;
			constexpr float length = 1.0f;
			constexpr float headHeight = 0.15f;
			constexpr float headLength = 0.25f;
			constexpr float lineLength = length - headLength;
			constexpr float squareSize = 0.25f;
			constexpr float squareOffset = 0.5f;

			std::array<Vector2, 9> verticesX = {
				Vector2(0.0f, thickness * 0.5f),
				Vector2(0.0f, -thickness * 0.5f),
				Vector2(lineLength, thickness * 0.5f),
				
				Vector2(0.0f, -thickness * 0.5f),
				Vector2(lineLength, -thickness * 0.5f),
				Vector2(lineLength, thickness * 0.5f),

				Vector2(lineLength, headHeight * 0.5f),
				Vector2(lineLength, -headHeight * 0.5f),
				Vector2(length, 0.0f),
			};

			renderer::RenderCommandMesh* renderMeshCommand = new renderer::RenderCommandMesh(verticesX.data(), nullptr, nullptr, (uint32_t)verticesX.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisX && pickingId != _pickingIdAxisX ? _materialInstanceAxisXNormal : _materialInstanceAxisXHightlight, _pickingIdAxisX);
			renderer::RenderQueue::GetInstance()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 9> verticesY = {
				Vector2(thickness * 0.5f, 0.0f),
				Vector2(-thickness * 0.5f, 0.0f),
				Vector2(thickness * 0.5f, lineLength),
				
				Vector2(-thickness * 0.5f, 0.0f),
				Vector2(-thickness * 0.5f, lineLength),
				Vector2(thickness * 0.5f, lineLength),

				Vector2(headHeight * 0.5f, lineLength),
				Vector2(-headHeight * 0.5f, lineLength),
				Vector2(0.0f, length),
			};

			renderMeshCommand = new renderer::RenderCommandMesh(verticesY.data(), nullptr, nullptr, (uint32_t)verticesY.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisY && pickingId != _pickingIdAxisY ? _materialInstanceAxisYNormal : _materialInstanceAxisYHightlight, _pickingIdAxisY);
			renderer::RenderQueue::GetInstance()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 6> verticesZ = {
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + squareSize * 0.5f),
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset +squareSize * 0.5f, squareOffset + squareSize * 0.5f),
				
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset + squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset + squareSize * 0.5f, squareOffset + squareSize * 0.5f),
			};

			renderMeshCommand = new renderer::RenderCommandMesh(verticesZ.data(), nullptr, nullptr, (uint32_t)verticesZ.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisZ &&  pickingId != _pickingIdAxisZ ? _materialInstanceAxisZNormal : _materialInstanceAxisZHightlight, _pickingIdAxisZ);
			renderer::RenderQueue::GetInstance()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 6> verticesCenter = {
				Vector2(-thickness * 0.5f, thickness * 0.5f),
				Vector2(-thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, thickness * 0.5f),
				
				Vector2(-thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, thickness * 0.5f),
			};

			renderMeshCommand = new renderer::RenderCommandMesh(verticesCenter.data(), nullptr, nullptr, (uint32_t)verticesCenter.size(), nullptr, 0, finalMatrix, _materialInstanceCenterNormal);
			renderer::RenderQueue::GetInstance()->PushRenderCommand(renderMeshCommand);
		}

		return changed;
	}

	/// @brief 
	/// @param mousePosition 
	/// @param viewport 
	/// @return 
	Vector2 Node2dComponentCustomEditor::GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetWidth() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetHeight();
		
		Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
