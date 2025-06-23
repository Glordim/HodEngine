#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentCustomEditor/TextureRendererComponentCustomEditor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/TextureRendererComponent.hpp>

#undef max

namespace hod::editor
{
	/// @brief 
	TextureRendererComponentCustomEditor::TextureRendererComponentCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.75f, 0.75f, 0.75f, 1.0f));
	}

	/// @brief 
	TextureRendererComponentCustomEditor::~TextureRendererComponentCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool TextureRendererComponentCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		game::TextureRendererComponent* textureRenderer = static_cast<game::TextureRendererComponent*>(component);
		if (textureRenderer != nullptr)
		{
			game::Node2dComponent* node2D = textureRenderer->GetOwner()->GetComponent<game::Node2dComponent>();
			if (node2D != nullptr)
			{
				Rect bb = textureRenderer->GetBoundingBox();

				std::array<Vector2, 5> vertices = {
					Vector2(-bb._size.GetX() * 0.5f, bb._size.GetY() * 0.5f),
					Vector2(bb._size.GetX() * 0.5f, bb._size.GetY() * 0.5f),
					Vector2(bb._size.GetX() * 0.5f, -bb._size.GetY() * 0.5f),
					Vector2(-bb._size.GetX() * 0.5f, -bb._size.GetY() * 0.5f),
					Vector2(-bb._size.GetX() * 0.5f, bb._size.GetY() * 0.5f),
				};

				renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
