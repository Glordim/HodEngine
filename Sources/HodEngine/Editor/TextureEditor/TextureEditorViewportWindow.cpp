#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorViewportWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorTab.hpp"

#include <HodEngine/Renderer/Resource/TextureResource.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/Entity.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Prefab.hpp"

#include <cmath>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(TextureEditorViewportWindow, "Viewport", true)

	/// @brief 
	/// @param editorTab 
	TextureEditorViewportWindow::TextureEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);
	}

	/// @brief 
	bool TextureEditorViewportWindow::Draw()
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
	void TextureEditorViewportWindow::DrawContent()
	{
		std::shared_ptr<renderer::TextureResource> textureResource = GetOwner<TextureEditorTab>()->GetTexture();
		renderer::Texture* texture = textureResource->GetTexture();

		float zoomFactor = GetOwner<TextureEditorTab>()->GetZoomFactor();
		float width = texture->GetWidth() * zoomFactor;
		float height = texture->GetHeight() * zoomFactor;

		ImVec2 cursor = ImGui::GetCursorPos();
		ImVec2 available = ImGui::GetContentRegionAvail();
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + available, IM_COL32(0, 0, 0, 255));

		cursor += ImVec2((available.x - width) * 0.5f, (available.y - height) * 0.5f);
		ImGui::SetCursorPos(cursor);
		ImGui::Image(Editor::GetInstance()->GetCheckerTexture(), ImVec2(width, height), ImVec2(0, 0), ImVec2(width / (4 * 8), height / (4 * 8)));
		ImGui::SetCursorPos(cursor);
		ImGui::Image(texture, ImVec2(width, height));

		if (ImGui::GetIO().MouseWheel)
		{
			zoomFactor += ImGui::GetIO().MouseWheel * 0.05f;
			GetOwner<TextureEditorTab>()->SetZoomFactor(zoomFactor);
		}
	}
}
