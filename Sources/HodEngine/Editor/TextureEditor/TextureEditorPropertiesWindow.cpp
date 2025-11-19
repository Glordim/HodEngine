#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorTab.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"

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
	DESCRIBE_REFLECTED_CLASS(TextureEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param editorTab 
	TextureEditorPropertiesWindow::TextureEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{

	}

	/// @brief 
	void TextureEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;

		std::shared_ptr<Asset> asset = GetOwner()->GetAsset();
		std::shared_ptr<TextureImporterSettings> textureImporterSettings = std::static_pointer_cast<TextureImporterSettings>(asset->GetMeta()._importerSettings);

		float valuePos = ImGui::GetContentRegionAvail().x * 0.4f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Generate mipmap");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		changed |= ImGui::Checkbox("##Generate mipmap", &textureImporterSettings->_generateMipmap);

		static const char* filterModeLabels[static_cast<std::underlying_type_t<renderer::FilterMode>>(renderer::FilterMode::Count)] = { "Nearest", "Linear" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Filter Mode");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##FilterMode", filterModeLabels[static_cast<std::underlying_type_t<renderer::FilterMode>>(textureImporterSettings->_filterMode)]))
		{
			for (uint32_t filterMode = 0; filterMode < static_cast<std::underlying_type_t<renderer::FilterMode>>(renderer::FilterMode::Count); ++filterMode)
			{
				if (ImGui::MenuItem(filterModeLabels[filterMode]))
				{
					textureImporterSettings->_filterMode = static_cast<renderer::FilterMode>(filterMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}

		static const char* wrapModeLabels[static_cast<std::underlying_type_t<renderer::WrapMode>>(renderer::WrapMode::Count)] = { "Clamp", "Repeat" };

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Wrap Mode");
		ImGui::SameLine(valuePos);
		ImGui::SetNextItemWidth(-1);
		if (ImGui::BeginCombo("##WrapMode", wrapModeLabels[static_cast<std::underlying_type_t<renderer::WrapMode>>(textureImporterSettings->_wrapMode)]))
		{
			for (uint32_t wrapMode = 0; wrapMode < static_cast<std::underlying_type_t<renderer::WrapMode>>(renderer::WrapMode::Count); ++wrapMode)
			{
				if (ImGui::MenuItem(wrapModeLabels[wrapMode]))
				{
					textureImporterSettings->_wrapMode = static_cast<renderer::WrapMode>(wrapMode);
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::BeginDisabled(asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			asset->Save();
			AssetDatabase::GetInstance()->Import(asset->GetPath());
		}
		ImGui::EndDisabled();

		if (changed)
		{
			GetOwner()->MarkAssetAsDirty();
		}
	}
}
