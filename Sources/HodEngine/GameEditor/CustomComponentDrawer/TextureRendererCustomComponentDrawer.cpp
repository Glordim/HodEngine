#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/GameEditor/CustomComponentDrawer/TextureRendererCustomComponentDrawer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Game/Components/Node2dComponent.hpp>
#include <HodEngine/Game/Components/TextureRendererComponent.hpp>
#include <HodEngine/Game/Entity.hpp>

#undef max

namespace hod::inline editor
{
	/// @brief
	TextureRendererCustomComponentDrawer::TextureRendererCustomComponentDrawer()
	{
		_materialInstance = Renderer::GetInstance()->CreateMaterialInstance(
			MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("ubo.color", Vector4(0.75f, 0.75f, 0.75f, 1.0f));
	}

	/// @brief
	TextureRendererCustomComponentDrawer::~TextureRendererCustomComponentDrawer()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param component
	/// @param projection
	/// @param view
	/// @param operation
	/// @return
	bool TextureRendererCustomComponentDrawer::OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected)
	{
		if (selected == false)
		{
			return false;
		}

		TextureRendererComponent* textureRenderer = static_cast<TextureRendererComponent*>(component);
		if (textureRenderer != nullptr)
		{
			Node2dComponent* node2D = textureRenderer->GetOwner()->GetComponent<Node2dComponent>();
			if (node2D != nullptr)
			{
				Rect bb = textureRenderer->GetBoundingBox();

				std::array<Vector2, 5> vertices = {
					Vector2(-bb._size.GetX() * 0.5f, bb._size.GetY() * 0.5f), Vector2(bb._size.GetX() * 0.5f, bb._size.GetY() * 0.5f),
					Vector2(bb._size.GetX() * 0.5f, -bb._size.GetY() * 0.5f), Vector2(-bb._size.GetX() * 0.5f, -bb._size.GetY() * 0.5f),
					Vector2(-bb._size.GetX() * 0.5f, bb._size.GetY() * 0.5f),
				};

				RenderCommandMesh* renderMeshCommand =
					DefaultAllocator::GetInstance().New<RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0,
				                                                                     node2D->GetWorldMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
				viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
			}
		}
		return false;
	}
}
