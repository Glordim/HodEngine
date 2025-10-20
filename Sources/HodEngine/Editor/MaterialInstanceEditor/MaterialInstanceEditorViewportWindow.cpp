#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorViewportWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"

#include <HodEngine/Renderer/Resource/MaterialInstanceResource.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#include <algorithm>
#include <cmath>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialInstanceEditorViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	MaterialInstanceEditorViewportWindow::MaterialInstanceEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);

		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_renderView.Init();
	}

	/// @brief
	MaterialInstanceEditorViewportWindow::~MaterialInstanceEditorViewportWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief
	bool MaterialInstanceEditorViewportWindow::Draw()
	{
		bool open = true;
		if (ImGui::Begin(GetIdentifier(), &open, GetFlags()) == true)
		{
			DrawContent();
		}
		if (open == false)
		{
			Close();
		}
		ImGui::End();
		return open;
	}

	/// @brief
	void MaterialInstanceEditorViewportWindow::DrawContent()
	{
		std::shared_ptr<hod::renderer::MaterialInstanceResource> materialInstanceResource = GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance();
		if (materialInstanceResource != nullptr)
		{
			renderer::MaterialInstance* materialInstance = materialInstanceResource->GetMaterialInstance();
			if (materialInstance != nullptr)
			{
				uint32_t resolutionWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
				uint32_t resolutionHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

				resolutionWidth = std::clamp(resolutionWidth, 2u, 16u * 1024u);
				resolutionHeight = std::clamp(resolutionHeight, 2u, 16u * 1024u);

				if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
				{
					renderer::Texture::CreateInfo createInfo;

					createInfo._allowReadWrite = false;
					_renderTarget->Init(resolutionWidth, resolutionHeight, createInfo); // todo error
				}

				if (_renderTarget->IsValid() == true)
				{
					_renderView.Prepare(_renderTarget, nullptr);

					Rect viewport;
					viewport._position.SetX(0);
					viewport._position.SetY(0);
					viewport._size.SetX((float)resolutionWidth);
					viewport._size.SetY((float)resolutionHeight);

					float aspect = (float)resolutionWidth / (float)resolutionHeight;

					_projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
					_view = Matrix4::Identity;

					_renderView.SetupCamera(_projection, _view, viewport);

					Vector2 previewSize = Vector2::One * 3;

					std::array<Vector2, 4> vertices = {
						Vector2(-0.5f * previewSize.GetX(), 0.5f * previewSize.GetY()),
						Vector2(0.5f * previewSize.GetX(), 0.5f * previewSize.GetY()),
						Vector2(0.5f * previewSize.GetX(), -0.5f * previewSize.GetY()),
						Vector2(-0.5f * previewSize.GetX(), -0.5f * previewSize.GetY()),
					};

					static std::array<Vector2, 4> uvs = {
						Vector2(0, 0),
						Vector2(1, 0),
						Vector2(1, 1),
						Vector2(0, 1),
					};

					static std::array<uint16_t, 3 * 2> indices = {
						0, 1, 2, 0, 2, 3,
					};

					renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
						vertices.data(), uvs.data(), nullptr, (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), Matrix4::Identity, materialInstance, 0);
					_renderView.PushRenderCommand(renderMeshCommand);

					renderer::Renderer::GetInstance()->PushRenderView(_renderView, false);
				}

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - resolutionWidth) * 0.5f);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - resolutionHeight) * 0.5f);
				ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));
			}
		}
	}
}
