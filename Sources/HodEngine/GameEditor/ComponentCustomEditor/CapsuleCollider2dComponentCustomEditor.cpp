#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/CapsuleCollider2dComponentCustomEditor.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/CapsuleCollider2dComponent.hpp>

#undef max

namespace hod::editor
{
	/// @brief 
	CapsuleCollider2dComponentCustomEditor::CapsuleCollider2dComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief 
	CapsuleCollider2dComponentCustomEditor::~CapsuleCollider2dComponentCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool CapsuleCollider2dComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		game::CapsuleCollider2dComponent* capsuleCollider2d = static_cast<game::CapsuleCollider2dComponent*>(component);
		if (capsuleCollider2d != nullptr)
		{
			game::Node2dComponent* node2D = capsuleCollider2d->GetOwner()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				Vector2 scale = node2D->GetScale();

				std::array<Vector2, 65> vertices;
				GeometryGenerator::CapsuleShape<64>(vertices, Vector2::Zero, capsuleCollider2d->GetHeight() * scale.GetY(), capsuleCollider2d->GetRadius() * scale.GetX());

				Matrix4 worldMatrix = node2D->GetWorldMatrix();
				worldMatrix.RemoveScale();

				Matrix4 localMatrix = Matrix4::Translation(capsuleCollider2d->GetOffset() * scale) * Matrix4::Rotation(capsuleCollider2d->GetRotation());

				renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, worldMatrix * localMatrix, _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
