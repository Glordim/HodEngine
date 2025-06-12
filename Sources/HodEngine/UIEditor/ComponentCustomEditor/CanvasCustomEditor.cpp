#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/ComponentCustomEditor/CanvasCustomEditor.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include <HodEngine/UI/Node.hpp>

#include <HodEngine/Game/Entity.hpp>

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
	CanvasCustomEditor::CanvasCustomEditor()
	{
		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		_materialInstance->SetVec4("UBO.color", Vector4(0.75f, 0.75f, 0.75f, 1.0f));
	}

	/// @brief 
	CanvasCustomEditor::~CanvasCustomEditor()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool CanvasCustomEditor::OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected)
	{
		ui::Node* node = component->GetOwner()->GetComponent<ui::Node>();
		if (node != nullptr)
		{
			Vector2 size = node->ComputeSize();

			std::array<Vector2, 5> vertices = {
				Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
				Vector2(size.GetX() * 0.5f, size.GetY() * 0.5f),
				Vector2(size.GetX() * 0.5f, -size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
			};

			static Vector4 selectedColor(0.75f, 0.75f, 0.75f, 1.0f);
			static Vector4 normalColor(0.45f, 0.45f, 0.45f, 1.0f);

			_materialInstance->SetVec4("UBO.color", selected ? selectedColor : normalColor);

			renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, node->ComputeCanvasMatrix(), _materialInstance, std::numeric_limits<uint32_t>::max() - 1);
			viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);
		}
		return false;
	}
}
