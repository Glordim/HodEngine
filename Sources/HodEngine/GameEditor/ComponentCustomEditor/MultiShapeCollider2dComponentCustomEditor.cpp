#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/MultiShapeCollider2dComponentCustomEditor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/Physics/2d/MultiShapeCollider2dComponent.hpp>
#include <HodEngine/Game/Entity.hpp>

#undef max

namespace hod::editor
{
	/// @brief
	MultiShapeCollider2dComponentCustomEditor::MultiShapeCollider2dComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("ubo.color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	/// @brief
	MultiShapeCollider2dComponentCustomEditor::~MultiShapeCollider2dComponentCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool MultiShapeCollider2dComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		game::MultiShapeCollider2dComponent* multiShapeCollider2d = static_cast<game::MultiShapeCollider2dComponent*>(component);
		if (multiShapeCollider2d != nullptr)
		{
			game::Node2dComponent* node2D = multiShapeCollider2d->GetOwner()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				Vector2 scale = node2D->GetScale();

				for (const game::MultiShapeCollider2dComponent::BoxShape& boxShape : multiShapeCollider2d->GetBoxShapes())
				{
					std::array<Vector2, 5> vertices = {
						Vector2(-boxShape._size.GetX() * 0.5f, boxShape._size.GetY() * 0.5f), Vector2(boxShape._size.GetX() * 0.5f, boxShape._size.GetY() * 0.5f),
						Vector2(boxShape._size.GetX() * 0.5f, -boxShape._size.GetY() * 0.5f), Vector2(-boxShape._size.GetX() * 0.5f, -boxShape._size.GetY() * 0.5f),
						Vector2(-boxShape._size.GetX() * 0.5f, boxShape._size.GetY() * 0.5f),
					};

					Matrix4 localMatrix = Matrix4::Translation(boxShape._origin) * Matrix4::Rotation(boxShape._angle);

					renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
						vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node2D->GetWorldMatrix() * localMatrix, _materialInstance,
						std::numeric_limits<uint32_t>::max() - 1);
					viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
				}

				for (const game::MultiShapeCollider2dComponent::CircleShape& circleShape : multiShapeCollider2d->GetCircleShapes())
				{
					std::array<Vector2, 65> vertices;
					GeometryGenerator::CircleShape<64>(vertices, circleShape._origin * scale, circleShape._radius * std::max(scale.GetX(), scale.GetY()));

					renderer::RenderCommandMesh* renderMeshCommand =
						DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0,
					                                                                     node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
					viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
				}
			}
		}
		return false;
	}
}
