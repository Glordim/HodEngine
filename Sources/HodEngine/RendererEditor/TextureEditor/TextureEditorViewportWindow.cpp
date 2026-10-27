#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorViewportWindow.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorTab.hpp"

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
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Game/PrefabUtility.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Prefab.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/Material.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Shader.hpp>
#include <HodEngine/RHI/VertexInput.hpp>

#include "Shader/TexturePreview_Fragment.hpp"
#include "Shader/TexturePreview_Vertex.hpp"

#include <cmath>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(TextureEditorViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param editorTab 
	TextureEditorViewportWindow::TextureEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);
		SetTitle("Viewport");

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
		CreateMaterial();
	}

	/// @brief
	TextureEditorViewportWindow::~TextureEditorViewportWindow()
	{
		DefaultAllocator::GetInstance().Delete(_material);
		DefaultAllocator::GetInstance().Delete(_vertexShader);
		DefaultAllocator::GetInstance().Delete(_fragmentShader);
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief Material of the TexturePreview shader, for the format of _renderTarget
	/// @return
	bool TextureEditorViewportWindow::CreateMaterial()
	{
		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		_vertexShader = rhiDevice->CreateShader(Shader::ShaderType::Vertex);
		if (_vertexShader->LoadFromIR(TexturePreview_Vertex, TexturePreview_Vertex_size, TexturePreview_Vertex_reflection, TexturePreview_Vertex_reflection_size) == false)
		{
			return false;
		}

		_fragmentShader = rhiDevice->CreateShader(Shader::ShaderType::Fragment);
		if (_fragmentShader->LoadFromIR(TexturePreview_Fragment, TexturePreview_Fragment_size, TexturePreview_Fragment_reflection, TexturePreview_Fragment_reflection_size) == false)
		{
			return false;
		}

		const VertexInput vertexInputs[2] = {
			VertexInput(0, 0, VertexInput::Format::R32G32_SFloat),
			VertexInput(1, 8, VertexInput::Format::R32G32_SFloat),
		};

		Material::CreateInfo createInfo;
		createInfo._vertexInputs = vertexInputs;
		createInfo._vertexInputCount = 2;
		createInfo._vertexShader = _vertexShader;
		createInfo._fragmentShader = _fragmentShader;
		createInfo._blendMode = BlendMode::Opaque; // The checker is blended in the shader

		_material = Material::Create(createInfo);
		return _material != nullptr;
	}

	/// @brief 
	bool TextureEditorViewportWindow::Draw()
	{
		bool open = true;
		PushDockTabStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		bool visible = ImGui::Begin(GetIdentifier(), &open, GetFlags());
		ImGui::PopStyleVar(2);
		PopDockTabStyle();
		if (visible)
		{
			DrawDockTabContextMenu();
			DrawContent();
			DrawHelper::DrawHiddenTabBarButton();
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
		TextureEditorTab*                textureEditorTab = GetOwner<TextureEditorTab>();
		std::shared_ptr<TextureResource> textureResource = textureEditorTab->GetTexture();
		if (textureResource == nullptr || _material == nullptr)
		{
			return;
		}
		Texture* texture = textureResource->GetTexture();

		// The render target covers the whole viewport: the image can be zoomed larger than it
		const ImVec2   available = ImGui::GetContentRegionAvail();
		const uint32_t resolutionWidth = Clamp((uint32_t)available.x, 2u, 16u * 1024u);
		const uint32_t resolutionHeight = Clamp((uint32_t)available.y, 2u, 16u * 1024u);
		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			_renderTarget->Init(resolutionWidth, resolutionHeight);
		}
		if (_renderTarget->IsValid() == false)
		{
			return;
		}

		// The input first, for the image to be drawn where it is this frame
		const ImVec2 viewportPosition = ImGui::GetCursorScreenPos();
		const ImVec2 viewportSize((float)resolutionWidth, (float)resolutionHeight);
		ImGui::InvisibleButton("##Viewport", viewportSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);

		const ImGuiIO& io = ImGui::GetIO();
		Vector2        panOffset = textureEditorTab->GetPanOffset();

		// Zoom changed out of the viewport since its last frame (slider of the menu bar): around the center of the view, not of the image
		if (_drawnZoomFactor > 0.0f && textureEditorTab->GetZoomFactor() != _drawnZoomFactor)
		{
			const float zoomRatio = textureEditorTab->GetZoomFactor() / _drawnZoomFactor;
			panOffset = Vector2(panOffset.GetX() * zoomRatio, panOffset.GetY() * zoomRatio);
			textureEditorTab->SetPanOffset(panOffset);
		}
		if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)))
		{
			panOffset = Vector2(panOffset.GetX() + io.MouseDelta.x, panOffset.GetY() + io.MouseDelta.y);
			textureEditorTab->SetPanOffset(panOffset);
			textureEditorTab->SetRecentering(false); // Taken back by the user
		}

		if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
		{
			textureEditorTab->SetTargetZoomFactor(textureEditorTab->GetTargetZoomFactor() * std::pow(1.1f, io.MouseWheel)); // 10% of the zoom per notch
			textureEditorTab->SetRecentering(false);
			_zoomAnchor = io.MousePos - viewportPosition;
		}

		// Eases towards the target zoom instead of snapping, like the UI editor, so each wheel notch feels smooth
		const float previousZoomFactor = textureEditorTab->GetZoomFactor();
		const float targetZoomFactor = textureEditorTab->GetTargetZoomFactor();
		const float easing = Min(1.0f, io.DeltaTime * 12.0f);
		if (previousZoomFactor != targetZoomFactor)
		{
			float zoomFactor = previousZoomFactor + (targetZoomFactor - previousZoomFactor) * easing;
			if (std::abs(targetZoomFactor - zoomFactor) < targetZoomFactor * 0.001f)
			{
				zoomFactor = targetZoomFactor;
			}
			textureEditorTab->SetZoomFactor(zoomFactor);

			if (textureEditorTab->IsRecentering() == false)
			{
				// Around the cursor of the wheel: the texel under it stays under it, all along
				const float  zoomRatio = textureEditorTab->GetZoomFactor() / previousZoomFactor;
				const ImVec2 imageCenter = viewportSize * 0.5f + ImVec2(panOffset.GetX(), panOffset.GetY());
				const ImVec2 anchorFromCenter = _zoomAnchor - imageCenter;
				panOffset = Vector2(panOffset.GetX() + anchorFromCenter.x * (1.0f - zoomRatio), panOffset.GetY() + anchorFromCenter.y * (1.0f - zoomRatio));
				textureEditorTab->SetPanOffset(panOffset);
			}
		}

		// Reset of the view: the image slides back to the center as it zooms back
		if (textureEditorTab->IsRecentering())
		{
			panOffset = Vector2(panOffset.GetX() * (1.0f - easing), panOffset.GetY() * (1.0f - easing));
			if (std::abs(panOffset.GetX()) < 0.5f && std::abs(panOffset.GetY()) < 0.5f && textureEditorTab->GetZoomFactor() == targetZoomFactor)
			{
				panOffset = Vector2(0.0f, 0.0f);
				textureEditorTab->SetRecentering(false);
			}
			textureEditorTab->SetPanOffset(panOffset);
		}

		const float zoomFactor = textureEditorTab->GetZoomFactor();
		_drawnZoomFactor = zoomFactor;
		const float width = texture->GetWidth() * zoomFactor;
		const float height = texture->GetHeight() * zoomFactor;

		// One unit per pixel, origin at the center of the render target. The image starts on a whole pixel: not blurred by the sampling at 100%
		const float left = std::floor((resolutionWidth - width) * 0.5f + panOffset.GetX()) - resolutionWidth * 0.5f;
		const float top = resolutionHeight * 0.5f - std::floor((resolutionHeight - height) * 0.5f + panOffset.GetY());

		StaticArray<Vector2, 4> vertices = {
			Vector2(left, top),
			Vector2(left + width, top),
			Vector2(left + width, top - height),
			Vector2(left, top - height),
		};

		static StaticArray<Vector2, 4> uvs = {
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1),
		};

		static StaticArray<uint16_t, 3 * 2> indices = {
			0, 1, 2, 0, 2, 3,
		};

		const uint8_t colorComponentMask = textureEditorTab->GetColorComponentMask();
		auto          isShown = [colorComponentMask](TextureEditorTab::ColorComponent component)
		{
			return (colorComponentMask & std::to_underlying(component)) ? 1.0f : 0.0f;
		};
		Vector4 channelMask(isShown(TextureEditorTab::ColorComponent::Red), isShown(TextureEditorTab::ColorComponent::Green), isShown(TextureEditorTab::ColorComponent::Blue),
		                    isShown(TextureEditorTab::ColorComponent::Alpha));

		// One per frame: the one of the previous frame may still be in use by the GPU
		FrameResources&   frameResources = Renderer::GetInstance()->GetCurrentFrameResources();
		MaterialInstance* materialInstance = MaterialInstance::Create(_material);
		materialInstance->SetTexture("image", texture);
		materialInstance->SetTexture("checker", Editor::GetInstance()->GetCheckerTexture());
		materialInstance->SetVec4("ubo.channelMask", channelMask);
		// The mip level shown is stretched over the size of the texture: its texels are larger
		const uint32_t mipLevel = Min((uint32_t)textureEditorTab->GetMipmapLevel(), texture->GetMipLevelCount() - 1);
		const float    mipWidth = (float)Texture::ComputeMipSize(texture->GetWidth(), mipLevel);
		const float    mipHeight = (float)Texture::ComputeMipSize(texture->GetHeight(), mipLevel);
		const Vector2  pixelsPerTexel(width / mipWidth, height / mipHeight);
		const float    texelZoomFactor = Min(pixelsPerTexel.GetX(), pixelsPerTexel.GetY());
		materialInstance->SetFloat("ubo.mipLevel", (float)mipLevel);
		materialInstance->SetVec2("ubo.textureSize", Vector2(mipWidth, mipHeight));
		materialInstance->SetVec2("ubo.pixelsPerTexel", pixelsPerTexel);
		// Texel grid: fades in from its min zoom to twice it, it would hide the texels if smaller. Alpha: how much a line covers the texel
		const TextureEditorTab::GridSettings& gridSettings = textureEditorTab->GetGridSettings();
		const float gridMinZoomFactor = gridSettings._minZoomFactor;
		const float gridStrength = gridSettings._visible ? gridSettings._opacity * Clamp((texelZoomFactor - gridMinZoomFactor) / gridMinZoomFactor, 0.0f, 1.0f) : 0.0f;
		Vector4     gridFirstColor(0.0f, 0.0f, 0.0f, 0.0f);
		Vector4     gridLastColor(0.0f, 0.0f, 0.0f, 0.0f);
		switch (gridSettings._style)
		{
			case TextureEditorTab::GridStyle::TwoTone:
				gridFirstColor = Vector4(0.0f, 0.0f, 0.0f, gridStrength);
				gridLastColor = Vector4(1.0f, 1.0f, 1.0f, gridStrength);
				break;
			case TextureEditorTab::GridStyle::Dark:
				gridFirstColor = Vector4(0.0f, 0.0f, 0.0f, gridStrength);
				break;
			case TextureEditorTab::GridStyle::Light:
				gridFirstColor = Vector4(1.0f, 1.0f, 1.0f, gridStrength);
				break;
			case TextureEditorTab::GridStyle::Color:
				gridFirstColor = Vector4(gridSettings._color.r, gridSettings._color.g, gridSettings._color.b, gridStrength);
				break;
			default:
				break;
		}
		materialInstance->SetVec4("ubo.gridFirstColor", gridFirstColor);
		materialInstance->SetVec4("ubo.gridLastColor", gridLastColor);
		// Checker squares aligned with the texels: the smallest power of two of texels at least 8 pixels wide, but 4 x 4 per texel at most
		materialInstance->SetFloat("ubo.checkerTexels", Max(0.25f, std::exp2(std::ceil(std::log2(8.0f / texelZoomFactor)))));
		frameResources.DeleteAfter(materialInstance);

		RenderView* renderView = frameResources.CreateRenderView();
		renderView->Init();
		renderView->Prepare(_renderTarget, nullptr);

		Rect viewport;
		viewport._position.SetX(0);
		viewport._position.SetY(0);
		viewport._size.SetX((float)resolutionWidth);
		viewport._size.SetY((float)resolutionHeight);

		const Matrix4 projection = Matrix4::OrthogonalProjection(-(float)resolutionWidth * 0.5f, (float)resolutionWidth * 0.5f, -(float)resolutionHeight * 0.5f,
		                                                         (float)resolutionHeight * 0.5f, -1024, 1024);
		renderView->SetupCamera(projection, Matrix4::Identity, viewport);

		RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<RenderCommandMesh>(
			vertices.Data(), uvs.Data(), nullptr, (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), Matrix4::Identity, materialInstance, 0, 0, true);
		renderView->PushRenderCommand(renderMeshCommand);

		ImGui::GetWindowDrawList()->AddImage(_renderTarget->GetColorTexture(), viewportPosition, viewportPosition + viewportSize);
	}
}
