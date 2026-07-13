#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorPropertiesWindow.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorViewportWindow.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialInstanceSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialInstanceSettings::_material, "_material");
		AddPropertyT(reflectionDescriptor, &MaterialInstanceSettings::_params, "_params")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief 
	MaterialInstanceEditorTab::MaterialInstanceEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_CIRCLE)
	{
		if (asset != nullptr)
		{
			const AssetContainer::DataBlockInfo* settingsDataBlock = _assetContainer.FindDataBlock("Settings");
			if (settingsDataBlock != nullptr)
			{
				Document           settingsDocument;
				DocumentReaderJson documentReader;
				if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == true)
				{
					Serializer::Deserialize(_materialInstanceSettings, settingsDocument.GetRootNode());
				}
			}

			_materialInstance = ResourceManager::GetInstance()->GetResource<MaterialInstanceResource>(asset->GetUid());
			if (_materialInstance != nullptr)
			{
				MaterialInstance* materialInstance = _materialInstance->GetMaterialInstance();
				if (materialInstance != nullptr)
				{
					const Material& material = materialInstance->GetMaterial();

					MaterialSerializationHelper::GenerateParameters(material, _parameters);
					for (const ShaderParameter& param : _parameters)
					{
						if (param._type == ShaderParameter::Type::Texture)
						{
							ShaderParamTexture textureParam;
							textureParam._name = param._name;
							textureParam._type = param._type;
							_textureParameters.push_back(textureParam);
						}
						else if (param._type == ShaderParameter::Type::Float2)
						{
							ShaderParamVec2 value;
							value._name = param._name;
							value._type = param._type;
							_vec2Parameters.push_back(value);
						}
						else if (param._type == ShaderParameter::Type::Float4)
						{
							ShaderParamVec4 value;
							value._name = param._name;
							value._type = param._type;
							_vec4Parameters.push_back(value);
						}
						else if (param._type != ShaderParameter::Type::Unknown)
						{
							ShaderParamScalar value;
							value._name = param._name;
							value._type = param._type;
							_scalarParameters.push_back(value);
						}
					}

					const DocumentNode* paramNode = _materialInstanceSettings._params.GetRootNode().GetFirstChild();
					while (paramNode != nullptr)
					{
						String name = paramNode->GetChild("Name")->GetString();
						ShaderParameter::Type type = static_cast<ShaderParameter::Type>(paramNode->GetChild("Type")->GetUInt8());
						switch (type)
						{
						case ShaderParameter::Type::Float:
							for (ShaderParamScalar& param : _scalarParameters)
							{
								if (param._name == name)
								{
									param._value.floatValue = paramNode->GetChild("Value")->GetFloat32();
									break;
								}
							}
							break;

						case ShaderParameter::Type::Float2:
							for (ShaderParamVec2& param : _vec2Parameters)
							{
								if (param._name == name)
								{
									Serializer::Deserialize(param._value, *paramNode->GetChild("Value"));
									break;
								}
							}
							break;

						case ShaderParameter::Type::Float4:
							for (ShaderParamVec4& param : _vec4Parameters)
							{
								if (param._name == name)
								{
									Serializer::Deserialize(param._value, *paramNode->GetChild("Value"));
									break;
								}
							}
							break;

						case ShaderParameter::Type::Texture:
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

					for (const ShaderParamScalar& param : _scalarParameters)
					{
						materialInstance->SetFloat("ubo." + param._name, param._value.floatValue);
					}
					for (const ShaderParamVec2& param : _vec2Parameters)
					{
						materialInstance->SetVec2("ubo." + param._name, param._value);
					}
					for (const ShaderParamVec4& param : _vec4Parameters)
					{
						materialInstance->SetVec4("ubo." + param._name, param._value);
					}
					for (const ShaderParamTexture& param : _textureParameters)
					{
						std::shared_ptr<TextureResource> textureResource = param._value.Lock();
						if (textureResource != nullptr)
						{
							materialInstance->SetTexture(param._name, textureResource->GetTexture());
						}
					}
				}
			}
		}
	}

	/// @brief 
	MaterialInstanceEditorTab::~MaterialInstanceEditorTab()
	{
	}

	/// @brief 
	/// @return 
	std::shared_ptr<MaterialInstanceResource> MaterialInstanceEditorTab::GetMaterialInstance() const
	{
		return _materialInstance;
	}

	/// @brief 
	/// @return 
	MaterialInstanceSettings& MaterialInstanceEditorTab::GetMaterialInstanceSettings()
	{
		return _materialInstanceSettings;
	}

	/// @brief 
	/// @return 
	Vector<MaterialInstanceEditorTab::ShaderParamScalar>& MaterialInstanceEditorTab::GetScalarParameters()
	{
		return _scalarParameters;
	}

	/// @brief 
	/// @return 
	Vector<MaterialInstanceEditorTab::ShaderParamTexture>& MaterialInstanceEditorTab::GetTextureParameters()
	{
		return _textureParameters;
	}

	/// @brief 
	/// @return 
	Vector<MaterialInstanceEditorTab::ShaderParamVec2>& MaterialInstanceEditorTab::GetVector2Parameters()
	{
		return _vec2Parameters;
	}

	/// @brief 
	/// @return 
	Vector<MaterialInstanceEditorTab::ShaderParamVec4>& MaterialInstanceEditorTab::GetVector4Parameters()
	{
		return _vec4Parameters;
	}

	/// @brief 
	/// @return 
	float MaterialInstanceEditorTab::GetZoomFactor() const
	{
		return _zoomFactor;
	}

	/// @brief 
	void MaterialInstanceEditorTab::SetZoomFactor(float zoomFactor)
	{
		_zoomFactor = std::max(0.01f, zoomFactor);
	}

	/// @brief 
	void MaterialInstanceEditorTab::CreateDefaultLayout()
	{
		MaterialInstanceEditorPropertiesWindow* propertiesWindow = OpenWindow<MaterialInstanceEditorPropertiesWindow>();
		MaterialInstanceEditorViewportWindow* viewportWindow = OpenWindow<MaterialInstanceEditorViewportWindow>();

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
	bool MaterialInstanceEditorTab::DrawContent()
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
	void MaterialInstanceEditorTab::DrawMenuBar()
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

	bool MaterialInstanceEditorTab::OnSave()
	{
		DocumentNode& paramsNode = _materialInstanceSettings._params.GetRootNode();
		paramsNode.Clear();
		for (const ShaderParamScalar& param : _scalarParameters)
		{
			DocumentNode& node = paramsNode.AddChild("");
			node.AddChild("Name").SetString(param._name);
			node.AddChild("Type").SetUInt8((uint8_t)param._type);
			node.AddChild("Value").SetFloat32(param._value.floatValue);
		}
		for (const ShaderParamVec2& param : _vec2Parameters)
		{
			DocumentNode& node = paramsNode.AddChild("");
			node.AddChild("Name").SetString(param._name);
			node.AddChild("Type").SetUInt8((uint8_t)param._type);
			Serializer::Serialize(param._value, node.AddChild("Value"));
		}
		for (const ShaderParamVec4& param : _vec4Parameters)
		{
			DocumentNode& node = paramsNode.AddChild("");
			node.AddChild("Name").SetString(param._name);
			node.AddChild("Type").SetUInt8((uint8_t)param._type);
			Serializer::Serialize(param._value, node.AddChild("Value"));
		}
		for (const ShaderParamTexture& param : _textureParameters)
		{
			DocumentNode& node = paramsNode.AddChild("");
			node.AddChild("Name").SetString(param._name);
			node.AddChild("Type").SetUInt8((uint8_t)param._type);
			Serializer::Serialize(param._value, node.AddChild("Value"));
		}

		Document settingsDocument;
		if (Serializer::Serialize(_materialInstanceSettings, settingsDocument.GetRootNode()) == false)
		{
			return false;
		}

		Stream& settingsStream = _assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(settingsDocument, settingsStream) == false)
		{
			return false;
		}

		return true;
	}
}
