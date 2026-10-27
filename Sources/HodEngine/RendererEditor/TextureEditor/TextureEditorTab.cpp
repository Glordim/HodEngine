#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorTab.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorPropertiesWindow.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorViewportWindow.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"
#include "HodEngine/Editor/InspectorWindow.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Widgets.hpp>

#include "HodEngine/Editor/DrawHelper.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::inline editor
{
	/// @brief 
	TextureEditorTab::TextureEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_CHECKERBOARD)
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
					Serializer::Deserialize(_textureSettings, settingsDocument.GetRootNode());
				}
			}

			_texture = ResourceManager::GetInstance()->GetResource<TextureResource>(asset->GetUid());
			ApplySamplerStates();
			if (_texture != nullptr)
			{
				_maxMipmapLevel = _texture->_mipLevelCount - 1; // cooked from the saved settings
			}

			_savedSettings = _textureSettings;
		}
	}

	/// @brief Reverts the resource, shared with whatever uses it, to what is saved: the live edits are not
	TextureEditorTab::~TextureEditorTab()
	{
		if (_texture != nullptr && IsDirty())
		{
			const bool mipmapsChanged = _textureSettings._generateMipmap != _savedSettings._generateMipmap ||
			                            (_savedSettings._generateMipmap && _textureSettings._wrapMode != _savedSettings._wrapMode);

			_textureSettings = _savedSettings;
			ApplySamplerStates();
			if (mipmapsChanged)
			{
				ApplyMipmaps();
			}
		}
	}

	/// @brief Generated as the TextureCooker does: the texture is shown, wherever it is used, as it will be once cooked
	void TextureEditorTab::ApplyMipmaps()
	{
		if (_texture == nullptr)
		{
			return;
		}

		const uint32_t pixelsSize = (uint32_t)_texture->_width * _texture->_height * 4;
		if (_sourcePixels.Size() != pixelsSize)
		{
			const AssetContainer::DataBlockInfo* pixelsDataBlock = _assetContainer.FindDataBlock("Pixels");
			if (pixelsDataBlock == nullptr)
			{
				return;
			}

			_sourcePixels.Resize(pixelsSize);
			if (pixelsDataBlock->_stream->Read(_sourcePixels.Data(), pixelsSize) != pixelsSize)
			{
				OUTPUT_ERROR("TextureEditorTab: Can't read the pixels of {}", GetAsset()->GetName());
				_sourcePixels.Clear();
				return;
			}
		}

		Vector<uint8_t> mipChain;
		const uint8_t   mipLevelCount = TextureCooker::BuildMipChain(_sourcePixels.Data(), _texture->_width, _texture->_height, _textureSettings, mipChain);
		if (mipLevelCount == 0 || _texture->SetPixels(mipChain.Data(), mipLevelCount) == false)
		{
			return;
		}

		_maxMipmapLevel = mipLevelCount - 1;
		_mipmapLevel = Min(_mipmapLevel, _maxMipmapLevel);
	}

	/// @brief The texture is sampled with the settings being edited, wherever it is used
	void TextureEditorTab::ApplySamplerStates()
	{
		if (_texture != nullptr)
		{
			_texture->SetSamplerStates(_textureSettings._filterMode, _textureSettings._wrapMode);
		}
	}

	/// @brief 
	/// @return 
	std::shared_ptr<TextureResource> TextureEditorTab::GetTexture() const
	{
		return _texture;
	}

	/// @brief 
	/// @return 
	float TextureEditorTab::GetZoomFactor() const
	{
		return _zoomFactor;
	}

	/// @brief 
	void TextureEditorTab::SetZoomFactor(float zoomFactor)
	{
		_zoomFactor = Clamp(zoomFactor, MinZoomFactor, MaxZoomFactor);
	}

	/// @brief
	/// @return
	float TextureEditorTab::GetTargetZoomFactor() const
	{
		return _targetZoomFactor;
	}

	/// @brief
	/// @param zoomFactor
	void TextureEditorTab::SetTargetZoomFactor(float zoomFactor)
	{
		_targetZoomFactor = Clamp(zoomFactor, MinZoomFactor, MaxZoomFactor);
	}

	/// @brief
	/// @return
	const Vector2& TextureEditorTab::GetPanOffset() const
	{
		return _panOffset;
	}

	/// @brief
	/// @param panOffset
	void TextureEditorTab::SetPanOffset(const Vector2& panOffset)
	{
		_panOffset = panOffset;
	}

	/// @brief 
	void TextureEditorTab::CreateDefaultLayout()
	{
		TextureEditorPropertiesWindow* propertiesWindow = OpenWindow<TextureEditorPropertiesWindow>();
		TextureEditorViewportWindow* viewportWindow = OpenWindow<TextureEditorViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(propertiesWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGuiDockNode* viewportNode = ImGui::DockBuilderGetNode(opposite);
		viewportNode->SetLocalFlags(viewportNode->LocalFlags | ImGuiDockNodeFlags_HiddenTabBar);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief 
	/// @return 
	bool TextureEditorTab::DrawContent()
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
	/// @return
	bool TextureEditorTab::OnSave()
	{
		Document settingsDocument;
		if (Serializer::Serialize(_textureSettings, settingsDocument.GetRootNode()) == false)
		{
			return false;
		}

		Stream& settingsStream = _assetContainer.AddDataBlock("Settings", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(settingsDocument, settingsStream) == false)
		{
			return false;
		}

		// Show the thumbnail the way the texture will be sampled (e.g. sharp pixel art)
		_assetContainer.SetThumbnailFilterMode(_textureSettings._filterMode);

		_savedSettings = _textureSettings;
		return true;
	}

	/// @brief
	/// @return
	const TextureEditorTab::GridSettings& TextureEditorTab::GetGridSettings() const
	{
		return _gridSettings;
	}

	/// @brief
	void TextureEditorTab::DrawMenuBarPopups()
	{
		DrawGridSettingsPopup();
	}

	/// @brief Style, opacity and zoom from which the texel grid is shown
	void TextureEditorTab::DrawGridSettingsPopup()
	{
		if (DrawHelper::BeginSettingsPopup("TextureGrid"))
		{
			static const char* styleLabels[std::to_underlying(GridStyle::Count)] = {"Two-tone", "Dark", "Light", "Color"};

			const float valuePos = ImGui::CalcTextSize("Visible from").x + ImGui::GetStyle().ItemSpacing.x * 2.0f;

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Style");
			ImGui::SameLine(valuePos);
			ImGui::SetNextItemWidth(120.0f);
			if (ImGui::BeginCombo("##Style", styleLabels[std::to_underlying(_gridSettings._style)]))
			{
				for (uint8_t style = 0; style < std::to_underlying(GridStyle::Count); ++style)
				{
					if (ImGui::Selectable(styleLabels[style], std::to_underlying(_gridSettings._style) == style))
					{
						_gridSettings._style = static_cast<GridStyle>(style);
					}
				}
				ImGui::EndCombo();
			}
			if (_gridSettings._style == GridStyle::Color)
			{
				ImGui::SameLine();
				float color[3] = {_gridSettings._color.r, _gridSettings._color.g, _gridSettings._color.b};
				ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 2.0f);
				if (ColorField("##Color", color, ImGuiColorEditFlags_NoAlpha))
				{
					_gridSettings._color = Color(color[0], color[1], color[2], 1.0f);
				}
			}

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Opacity");
			ImGui::SameLine(valuePos);
			ImGui::SetNextItemWidth(120.0f);
			float opacityPercent = _gridSettings._opacity * 100.0f;
			if (ImGui::SliderFloat("##Opacity", &opacityPercent, 0.0f, 100.0f, "%.0f%%", ImGuiSliderFlags_AlwaysClamp))
			{
				_gridSettings._opacity = opacityPercent / 100.0f;
			}

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Visible from");
			ImGui::SameLine(valuePos);
			ImGui::SetNextItemWidth(120.0f);
			float minZoomPercent = _gridSettings._minZoomFactor * 100.0f;
			if (ImGui::SliderFloat("##VisibleFrom", &minZoomPercent, 100.0f, MaxZoomFactor * 100.0f, "%.0f%%", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp))
			{
				_gridSettings._minZoomFactor = minZoomPercent / 100.0f;
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip("Zoom from which the grid fades in, fully visible at twice this zoom");
			}

			ImGui::EndPopup();
		}
	}

	/// @brief
	void TextureEditorTab::DrawMenuBar()
	{
		ImGui::SameLine(0.0f, 8.0f);

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		bool toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Red));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.8f, 0.0f, 0.0f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("R", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Red));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Red);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 4.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Green));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("G", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Green));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Green);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 4.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Blue));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.0f, 0.0f, 0.8f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(0.0f, 0.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(0.0f, 0.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("B", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Blue));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Blue);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 4.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));

		toggled = (_colorComponentMask & std::to_underlying(ColorComponent::Alpha));
		ImGui::PushStyleColor(ImGuiCol_Text, toggled ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, toggled ? ImVec4(0.8f, 0.8f, 0.8f, 1.0f) : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, toggled ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toggled ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::Button("A", ImVec2(32, 28)) == true)
		{
			if (toggled)
			{
				_colorComponentMask &= ~(std::to_underlying(ColorComponent::Alpha));
			}
			else
			{
				_colorComponentMask |= std::to_underlying(ColorComponent::Alpha);
			}
		}
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f + (28.0f - ImGui::GetFrameHeight()) * 0.5f)); // centered on the 28 pixels high buttons

		// The frame padding of the editor style (ImGuiManager), as the toggles of the other viewports: the menu bar one is wider for its icon buttons
		ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 5.0f);
		DrawHelper::DrawToggleWithSettings(ICON_MDI_GRID " Grid", "texel grid", "TextureGrid", _gridSettings._visible); // Its popup: DrawMenuBarPopups
		ImGui::PopStyleVar();

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		if (_maxMipmapLevel == 0)
		{
			ImGui::BeginDisabled();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("No Mipmap");
			ImGui::EndDisabled();
		}
		else
		{
			uint8_t minMipmapLevel = 0;
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Mipmap");
			ImGui::SameLine(0.0f, 8.0f);
			cursorPos = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

			ImGui::SetNextItemWidth(160);
			ImGui::SliderScalar("##MipmapLevel", ImGuiDataType_U8, &_mipmapLevel, &minMipmapLevel, &_maxMipmapLevel);
		}

		ImGui::SameLine(0.0f, 8.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Zoom");
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));

		ImGui::SetNextItemWidth(160);
		// Logarithmic: as precise around 10% as around 1000%, like the zoom of the mouse wheel
		if (ImGui::SliderFloat("##Zoom", &_zoomFactor, MinZoomFactor, MaxZoomFactor, "", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp))
		{
			_targetZoomFactor = _zoomFactor; // Follows the slider at once, no easing
		}
		ImGui::SameLine(0.0f, 8.0f);
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 6.0f));
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%u%%", static_cast<uint32_t>(_zoomFactor * 100.0f));

		ImGui::SameLine(0.0f, 8.0f);
		cursorPos.x += ImGui::CalcTextSize("6400%").x; // the widest (MaxZoomFactor)
		ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, 2.0f));
		// Resets the whole view: zoom and pan
		ImGui::BeginDisabled(_zoomFactor == 1.0f && _targetZoomFactor == 1.0f && _panOffset.GetX() == 0.0f && _panOffset.GetY() == 0.0f);
		if (ImGui::Button(ICON_MDI_UNDO, ImVec2(0, 28)) == true)
		{
			// Eased by the viewport, like the zoom of the mouse wheel
			_targetZoomFactor = 1.0f;
			_recentering = true;
		}
		ImGui::EndDisabled();
	}
}
