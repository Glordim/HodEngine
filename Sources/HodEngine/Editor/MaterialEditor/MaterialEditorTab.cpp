#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialEditorTab.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialEditorPropertiesWindow.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialEditorViewportWindow.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/Resource/ResourceManager.hpp>

#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"

namespace hod::editor
{
	/// @brief 
	MaterialEditorTab::MaterialEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_CIRCLE_OPACITY)
	{
		if (asset != nullptr)
		{
			_material = ResourceManager::GetInstance()->GetResource<renderer::MaterialResource>(asset->GetMeta()._uid);
			if (_material != nullptr)
			{
				std::shared_ptr<MaterialImporterSettings> materialImporterSettings = std::static_pointer_cast<MaterialImporterSettings>(asset->GetMeta()._importerSettings);

				const renderer::Material* material = _material->GetMaterial();
				if (material != nullptr)
				{
					renderer::MaterialSerializationHelper::GenerateParameters(*material, _parameters);
					for (const renderer::ShaderParameter& param : _parameters)
					{
						if (param._type == renderer::ShaderParameter::Type::Texture)
						{
							ShaderParamTexture textureParam;
							textureParam._name = param._name;
							textureParam._type = param._type;
							_textureParameters.push_back(textureParam);
						}
						else if (param._type == renderer::ShaderParameter::Type::Float2)
						{
							ShaderParamVec2 value;
							value._name = param._name;
							value._type = param._type;
							_vec2Parameters.push_back(value);
						}
						else if (param._type == renderer::ShaderParameter::Type::Float4)
						{
							ShaderParamVec4 value;
							value._name = param._name;
							value._type = param._type;
							_vec4Parameters.push_back(value);
						}
						else if (param._type != renderer::ShaderParameter::Type::Unknown)
						{
							ShaderParamScalar value;
							value._name = param._name;
							value._type = param._type;
							_scalarParameters.push_back(value);
						}
					}
				}

				const Document::Node* paramNode = materialImporterSettings->_defaultInstanceParams.GetRootNode().GetFirstChild();
				while (paramNode != nullptr)
				{
					String name = paramNode->GetChild("Name")->GetString();
					renderer::ShaderParameter::Type type = static_cast<renderer::ShaderParameter::Type>(paramNode->GetChild("Type")->GetUInt8());
					switch (type)
					{
					case renderer::ShaderParameter::Type::Float:
						for (ShaderParamScalar& param : _scalarParameters)
						{
							if (param._name == name)
							{
								param._value.floatValue = paramNode->GetChild("Value")->GetFloat32();
								break;
							}
						}
						break;

					case renderer::ShaderParameter::Type::Float2:
						for (ShaderParamVec2& param : _vec2Parameters)
						{
							if (param._name == name)
							{
								Serializer::Deserialize(param._value, *paramNode->GetChild("Value"));
								break;
							}
						}
						break;

					case renderer::ShaderParameter::Type::Float4:
						for (ShaderParamVec4& param : _vec4Parameters)
						{
							if (param._name == name)
							{
								Serializer::Deserialize(param._value, *paramNode->GetChild("Value"));
								break;
							}
						}
						break;

					case renderer::ShaderParameter::Type::Texture:
						for (ShaderParamTexture& param : _textureParameters)
						{
							if (param._name == name)
							{
								Serializer::Deserialize(param._value, *paramNode->GetChild("Value"));
								break;
							}
						}
						break;
					
					default:
						break;
					}
					paramNode = paramNode->GetNextSibling();
				}
			}
		}
	}

	/// @brief 
	MaterialEditorTab::~MaterialEditorTab()
	{
	}

	/// @brief 
	/// @return 
	std::shared_ptr<renderer::MaterialResource> MaterialEditorTab::GetMaterial() const
	{
		return _material;
	}

	/// @brief 
	/// @return 
	Vector<MaterialEditorTab::ShaderParamScalar>& MaterialEditorTab::GetScalarParameters()
	{
		return _scalarParameters;
	}

	/// @brief 
	/// @return 
	Vector<MaterialEditorTab::ShaderParamTexture>& MaterialEditorTab::GetTextureParameters()
	{
		return _textureParameters;
	}

	/// @brief 
	/// @return 
	Vector<MaterialEditorTab::ShaderParamVec2>& MaterialEditorTab::GetVector2Parameters()
	{
		return _vec2Parameters;
	}

	/// @brief 
	/// @return 
	Vector<MaterialEditorTab::ShaderParamVec4>& MaterialEditorTab::GetVector4Parameters()
	{
		return _vec4Parameters;
	}

	/// @brief 
	/// @return 
	float MaterialEditorTab::GetZoomFactor() const
	{
		return _zoomFactor;
	}

	/// @brief 
	void MaterialEditorTab::SetZoomFactor(float zoomFactor)
	{
		_zoomFactor = std::max(0.01f, zoomFactor);
	}

	/// @brief 
	void MaterialEditorTab::CreateDefaultLayout()
	{
		MaterialEditorPropertiesWindow* propertiesWindow = OpenWindow<MaterialEditorPropertiesWindow>();
		MaterialEditorViewportWindow* viewportWindow = OpenWindow<MaterialEditorViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief 
	/// @return 
	bool MaterialEditorTab::DrawContent()
	{
		// todo override GetIdentifier ?
		/*
		if (_asset != nullptr && _assetWasDirty == true && _asset->IsDirty() == false)
		{
			_assetWasDirty = false;
			SetTitle(_asset->GetName());
		}
		*/
		//

		return true;
	}

	/// @brief 
	void MaterialEditorTab::DrawMenuBar()
	{
		ImGui::SameLine(0.0f, 8.0f);
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Zoom");
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		ImGui::SetNextItemWidth(160);
		ImGui::SliderFloat("##Zoom", &_zoomFactor, 0.01f, 2.0f, "");
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%u%%", static_cast<uint32_t>(_zoomFactor * 100.0f));

		ImGui::SameLine(0.0f, 8.0f);
		cursorPos.x += ImGui::CalcTextSize("100%").x;
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));
		ImGui::BeginDisabled(_zoomFactor == 1.0f);
		if (ImGui::Button(ICON_MDI_UNDO, ImVec2(0, 28)) == true)
		{
			_zoomFactor = 1.0f;
		}
		ImGui::EndDisabled();
	}
}
