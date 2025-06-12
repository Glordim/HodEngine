#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/ComponentCustomEditor/NodeCustomEditor.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include <HodEngine/UI/Node.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#include "HodEngine/Editor/ViewportWindow.hpp"

#undef max

namespace hod::editor
{
	/// @brief 
	NodeCustomEditor::NodeCustomEditor()
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

		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.75f, 0.75f, 0.75f, 1.0f));
	}

	/// @brief 
	NodeCustomEditor::~NodeCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstanceCenterNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceCenterHightlight);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisXNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisXHightlight);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisYNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisYHightlight);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisZNormal);
		DefaultAllocator::GetInstance().Delete(_materialInstanceAxisZHightlight);

		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool NodeCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		bool changed = false;

		ui::Node* node = static_cast<ui::Node*>(component);
		if (node != nullptr)
		{
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
						_pickingOffset = mouseWorldPos - node->GetPosition();
					}
				}
			}
			else
			{
				_movingAxis = 0;
			}

			Matrix4 finalMatrix = Matrix4::Translation(node->ComputeCanvasMatrix().GetTranslation()); // Backup gizmo matrix before modification to avoid desync/lag
			
			if (_movingAxis != 0 && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
			{
				Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
				Vector2 previousPosition = node->GetPosition();
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
					node->SetPosition(newPosition);
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

			renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesX.data(), nullptr, nullptr, (uint32_t)verticesX.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisX && pickingId != _pickingIdAxisX ? _materialInstanceAxisXNormal : _materialInstanceAxisXHightlight, std::numeric_limits<uint32_t>::max(), _pickingIdAxisX);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

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

			renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesY.data(), nullptr, nullptr, (uint32_t)verticesY.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisY && pickingId != _pickingIdAxisY ? _materialInstanceAxisYNormal : _materialInstanceAxisYHightlight, std::numeric_limits<uint32_t>::max(), _pickingIdAxisY);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 6> verticesZ = {
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + squareSize * 0.5f),
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset +squareSize * 0.5f, squareOffset + squareSize * 0.5f),
				
				Vector2(squareOffset + -squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset + squareSize * 0.5f, squareOffset + -squareSize * 0.5f),
				Vector2(squareOffset + squareSize * 0.5f, squareOffset + squareSize * 0.5f),
			};

			renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesZ.data(), nullptr, nullptr, (uint32_t)verticesZ.size(), nullptr, 0, finalMatrix, _movingAxis != _pickingIdAxisZ && pickingId != _pickingIdAxisZ ? _materialInstanceAxisZNormal : _materialInstanceAxisZHightlight, std::numeric_limits<uint32_t>::max(), _pickingIdAxisZ);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

			std::array<Vector2, 6> verticesCenter = {
				Vector2(-thickness * 0.5f, thickness * 0.5f),
				Vector2(-thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, thickness * 0.5f),
				
				Vector2(-thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, -thickness * 0.5f),
				Vector2(thickness * 0.5f, thickness * 0.5f),
			};

			renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(verticesCenter.data(), nullptr, nullptr, (uint32_t)verticesCenter.size(), nullptr, 0, finalMatrix, _materialInstanceCenterNormal, std::numeric_limits<uint32_t>::max());
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);
		}

		Vector2 size = node->ComputeSize();

		std::array<Vector2, 5> vertices = {
			Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
			Vector2(size.GetX() * 0.5f, size.GetY() * 0.5f),
			Vector2(size.GetX() * 0.5f, -size.GetY() * 0.5f),
			Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f),
			Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
		};

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node->ComputeCanvasMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
		viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

		return changed;
	}

	/// @brief 
	/// @param mousePosition 
	/// @param viewport 
	/// @return 
	Vector2 NodeCustomEditor::GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetWidth() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetHeight();
		
		Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
