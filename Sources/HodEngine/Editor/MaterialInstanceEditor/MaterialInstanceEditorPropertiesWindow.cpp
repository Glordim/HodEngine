#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"
#include "HodEngine/Editor/Importer/MaterialImporter.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitImporterCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/PropertyCustomEditor/Vector2CustomEditor.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/Vector4CustomEditor.hpp"
#include "HodEngine/Editor/PropertyCustomEditor/WeakResourceCustomEditor.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Editor/Importer/MaterialImporter.hpp"

#include <HodEngine/Renderer/Resource/MaterialInstanceResource.hpp>
#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <cmath>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(MaterialInstanceEditorPropertiesWindow, "Properties", true)

	/// @brief 
	/// @param editorTab 
	MaterialInstanceEditorPropertiesWindow::MaterialInstanceEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{

	}

	/// @brief 
	void MaterialInstanceEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;

		std::shared_ptr<Asset> asset = GetOwner()->GetAsset();
		std::shared_ptr<MaterialImporterSettings> materialImporterSettings = std::static_pointer_cast<MaterialImporterSettings>(asset->GetMeta()._importerSettings);
		MaterialInstanceAsset& materialInstanceAsset  = GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstanceAsset();

		if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EditorReflectedObject reflectedObject(&materialInstanceAsset, &materialInstanceAsset.GetReflectionDescriptorV(), nullptr);
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		if (ImGui::CollapsingHeader("Params", ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::shared_ptr<renderer::MaterialInstanceResource> materialInstanceResource = GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance();
			for (MaterialInstanceEditorTab::ShaderParamScalar& scalarParameter : GetOwner<MaterialInstanceEditorTab>()->GetScalarParameters())
			{
				switch (scalarParameter._type)
				{
				case renderer::ShaderParameter::Type::Float:
				{
					ImGui::PushID(&scalarParameter);
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted(scalarParameter._name.c_str());
					ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
					ReflectionPropertyVariable reflectionPropertyVariable(ReflectionPropertyVariable::Type::Float32, 0, scalarParameter._name.c_str(), nullptr, nullptr);
					EditorReflectedProperty editorProperty(&scalarParameter._value.floatValue, nullptr, &reflectionPropertyVariable, nullptr);
					if (PropertyDrawer::DrawProperty(editorProperty))
					{
						GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance()->GetMaterialInstance()->SetFloat("UBO." + scalarParameter._name, scalarParameter._value.floatValue);
						changed = true;
					}
					ImGui::PopID();
				}
				break;

				default:
				{
				}
				break;
				}
			}

			for (MaterialInstanceEditorTab::ShaderParamVec2& vec2Parameter : GetOwner<MaterialInstanceEditorTab>()->GetVector2Parameters())
			{
				ImGui::PushID(&vec2Parameter);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(vec2Parameter._name.c_str());
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
				if (Vector2CustomEditor::Draw(vec2Parameter._value))
				{
					GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance()->GetMaterialInstance()->SetVec2("UBO." + vec2Parameter._name, vec2Parameter._value);
					changed = true;
				}
				ImGui::PopID();
			}
			for (MaterialInstanceEditorTab::ShaderParamVec4& vec4Parameter : GetOwner<MaterialInstanceEditorTab>()->GetVector4Parameters())
			{
				ImGui::PushID(&vec4Parameter);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(vec4Parameter._name.c_str());
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
				if (Vector4CustomEditor::Draw(vec4Parameter._value))
				{
					GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance()->GetMaterialInstance()->SetVec4("UBO." + vec4Parameter._name, vec4Parameter._value);
					changed = true;
				}
				ImGui::PopID();
			}
			for (MaterialInstanceEditorTab::ShaderParamTexture& textureParameter : GetOwner<MaterialInstanceEditorTab>()->GetTextureParameters())
			{
				ImGui::PushID(&textureParameter);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(textureParameter._name.c_str());
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
				if (WeakResourceCustomEditor::Draw(textureParameter._value))
				{
					GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance()->GetMaterialInstance()->SetTexture(textureParameter._name, textureParameter._value.Lock()->GetTexture());
					changed = true;
				}
				ImGui::PopID();
			}
		}

		ImGui::BeginDisabled(asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			std::shared_ptr<renderer::MaterialInstanceResource> materialInstanceResource = GetOwner<MaterialInstanceEditorTab>()->GetMaterialInstance();
			if (materialInstanceResource != nullptr)
			{
				renderer::MaterialInstance* materialInstance = materialInstanceResource->GetMaterialInstance();
				if (materialInstance != nullptr)
				{
					materialInstanceAsset._params.GetRootNode().Clear();
					for (const MaterialInstanceEditorTab::ShaderParamScalar& scalarParameter : GetOwner<MaterialInstanceEditorTab>()->GetScalarParameters())
					{
						Document::Node& node = materialInstanceAsset._params.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(scalarParameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)scalarParameter._type);
						node.AddChild("Value").SetFloat32(scalarParameter._value.floatValue);
					}
					for (const MaterialInstanceEditorTab::ShaderParamVec2& vec2Parameter : GetOwner<MaterialInstanceEditorTab>()->GetVector2Parameters())
					{
						Document::Node& node = materialInstanceAsset._params.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(vec2Parameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)vec2Parameter._type);
						Serializer::Serialize(vec2Parameter._value, node.AddChild("Value"));
					}
					for (const MaterialInstanceEditorTab::ShaderParamVec4& vec4Parameter : GetOwner<MaterialInstanceEditorTab>()->GetVector4Parameters())
					{
						Document::Node& node = materialInstanceAsset._params.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(vec4Parameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)vec4Parameter._type);
						Serializer::Serialize(vec4Parameter._value, node.AddChild("Value"));
					}
					for (const MaterialInstanceEditorTab::ShaderParamTexture& textureParameter : GetOwner<MaterialInstanceEditorTab>()->GetTextureParameters())
					{
						Document::Node& node = materialInstanceAsset._params.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(textureParameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)textureParameter._type);
						Serializer::Serialize(textureParameter._value, node.AddChild("Value"));
					}
					//Serializer::Serialize(material->GetReflectionDescriptorForParameters(), static_cast<void*>(materialInstanceAsset._paramsBuffer), materialInstanceAsset._defaultInstanceParams.GetRootNode());
				}
			}
			asset->Save(&materialInstanceAsset, &materialInstanceAsset.GetReflectionDescriptorV());
			AssetDatabase::GetInstance()->Import(asset->GetPath());
		}
		ImGui::EndDisabled();

		if (changed)
		{
			GetOwner()->MarkAssetAsDirty();
		}
	}
}
