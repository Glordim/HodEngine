#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialEditorTab.hpp"
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
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Editor/Importer/MaterialImporter.hpp"

#include <HodEngine/Renderer/Resource/MaterialResource.hpp>
#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <cmath>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(MaterialEditorPropertiesWindow, "Properties", true)

	/// @brief 
	/// @param editorTab 
	MaterialEditorPropertiesWindow::MaterialEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{

	}

	/// @brief 
	void MaterialEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;

		std::shared_ptr<Asset> asset = GetOwner()->GetAsset();
		std::shared_ptr<MaterialImporterSettings> materialImporterSettings = std::static_pointer_cast<MaterialImporterSettings>(asset->GetMeta()._importerSettings);
		MaterialAsset& materialAsset  = GetOwner<MaterialEditorTab>()->GetMaterialAsset();

		if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EditorReflectedObject reflectedObject(&materialAsset, &materialAsset.GetReflectionDescriptorV(), nullptr, this);
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		if (ImGui::CollapsingHeader("Default Params", ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::shared_ptr<renderer::MaterialResource> materialResource = GetOwner<MaterialEditorTab>()->GetMaterial();
			for (MaterialEditorTab::ShaderParamScalar& scalarParameter : GetOwner<MaterialEditorTab>()->GetScalarParameters())
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
						GetOwner<MaterialEditorTab>()->GetMaterial()->GetMaterial()->EditDefaultInstance()->SetFloat("ubo." + scalarParameter._name, scalarParameter._value.floatValue);
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

			for (MaterialEditorTab::ShaderParamVec2& vec2Parameter : GetOwner<MaterialEditorTab>()->GetVector2Parameters())
			{
				ImGui::PushID(&vec2Parameter);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(vec2Parameter._name.c_str());
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
				EditorReflectedObject editorObject(&vec2Parameter._value, &Vector2::GetReflectionDescriptor(), nullptr, nullptr);
				if (PropertyDrawer::DrawDescriptor(editorObject))
				{
					GetOwner<MaterialEditorTab>()->GetMaterial()->GetMaterial()->EditDefaultInstance()->SetVec2("ubo." + vec2Parameter._name, vec2Parameter._value);
					changed = true;
				}
				ImGui::PopID();
			}
			for (MaterialEditorTab::ShaderParamVec4& vec4Parameter : GetOwner<MaterialEditorTab>()->GetVector4Parameters())
			{
				ImGui::PushID(&vec4Parameter);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(vec4Parameter._name.c_str());
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
				EditorReflectedObject editorObject(&vec4Parameter._value, &Vector4::GetReflectionDescriptor(), nullptr, nullptr);
				if (PropertyDrawer::DrawDescriptor(editorObject))
				{
					GetOwner<MaterialEditorTab>()->GetMaterial()->GetMaterial()->EditDefaultInstance()->SetVec4("ubo." + vec4Parameter._name, vec4Parameter._value);
					changed = true;
				}
				ImGui::PopID();
			}
			for (MaterialEditorTab::ShaderParamTexture& textureParameter : GetOwner<MaterialEditorTab>()->GetTextureParameters())
			{
				ImGui::PushID(&textureParameter);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(textureParameter._name.c_str());
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);
				EditorReflectedObject editorObject(&textureParameter._value, &WeakResourceBase::GetReflectionDescriptor(), nullptr, nullptr);
				if (PropertyDrawer::DrawDescriptor(editorObject))
				{
					std::shared_ptr<hod::renderer::TextureResource> textureResource = textureParameter._value.Lock();
					if (textureResource != nullptr)
					{
						GetOwner<MaterialEditorTab>()->GetMaterial()->GetMaterial()->EditDefaultInstance()->SetTexture(textureParameter._name, textureResource->GetTexture());
					}
					changed = true;
				}
				ImGui::PopID();
			}
		}

		ImGui::BeginDisabled(asset->IsDirty() == false);
		if (ImGui::Button("Apply"))
		{
			std::shared_ptr<renderer::MaterialResource> materialResource = GetOwner<MaterialEditorTab>()->GetMaterial();
			if (materialResource != nullptr)
			{
				renderer::Material* material = materialResource->GetMaterial();
				if (material != nullptr)
				{
					materialAsset._defaultInstanceParams.GetRootNode().Clear();
					for (const MaterialEditorTab::ShaderParamScalar& scalarParameter : GetOwner<MaterialEditorTab>()->GetScalarParameters())
					{
						Document::Node& node = materialAsset._defaultInstanceParams.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(scalarParameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)scalarParameter._type);
						node.AddChild("Value").SetFloat32(scalarParameter._value.floatValue);
					}
					for (const MaterialEditorTab::ShaderParamVec2& vec2Parameter : GetOwner<MaterialEditorTab>()->GetVector2Parameters())
					{
						Document::Node& node = materialAsset._defaultInstanceParams.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(vec2Parameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)vec2Parameter._type);
						Serializer::Serialize(vec2Parameter._value, node.AddChild("Value"));
					}
					for (const MaterialEditorTab::ShaderParamVec4& vec4Parameter : GetOwner<MaterialEditorTab>()->GetVector4Parameters())
					{
						Document::Node& node = materialAsset._defaultInstanceParams.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(vec4Parameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)vec4Parameter._type);
						Serializer::Serialize(vec4Parameter._value, node.AddChild("Value"));
					}
					for (const MaterialEditorTab::ShaderParamTexture& textureParameter : GetOwner<MaterialEditorTab>()->GetTextureParameters())
					{
						Document::Node& node = materialAsset._defaultInstanceParams.GetRootNode().AddChild("");
						node.AddChild("Name").SetString(textureParameter._name);
						node.AddChild("Type").SetUInt8((uint8_t)textureParameter._type);
						Serializer::Serialize(textureParameter._value, node.AddChild("Value"));
					}
					//Serializer::Serialize(material->GetReflectionDescriptorForParameters(), static_cast<void*>(materialAsset._paramsBuffer), materialAsset._defaultInstanceParams.GetRootNode());
				}
			}
			asset->Save(&materialAsset, &materialAsset.GetReflectionDescriptorV());
			AssetDatabase::GetInstance()->Import(asset->GetPath());
		}
		ImGui::EndDisabled();

		if (changed)
		{
			GetOwner()->MarkAssetAsDirty();
		}
	}
}
