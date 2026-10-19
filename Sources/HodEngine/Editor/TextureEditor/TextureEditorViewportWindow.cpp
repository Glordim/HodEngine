#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TextureEditor/TextureEditorViewportWindow.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
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
			Texture::CreateInfo createInfo;
			createInfo._allowReadWrite = false;
			_renderTarget->Init(resolutionWidth, resolutionHeight, createInfo);
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
		if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)))
		{
			panOffset = Vector2(panOffset.GetX() + io.MouseDelta.x, panOffset.GetY() + io.MouseDelta.y);
			textureEditorTab->SetPanOffset(panOffset);
		}

		if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
		{
			// Around the cursor: the texel under it stays under it
			const float previousZoomFactor = textureEditorTab->GetZoomFactor();
			textureEditorTab->SetZoomFactor(previousZoomFactor * std::pow(1.1f, io.MouseWheel)); // 10% of the current zoom per notch
			const float zoomRatio = textureEditorTab->GetZoomFactor() / previousZoomFactor;

			const ImVec2 imageCenter = viewportPosition + viewportSize * 0.5f + ImVec2(panOffset.GetX(), panOffset.GetY());
			const ImVec2 cursorFromCenter = io.MousePos - imageCenter;
			panOffset = Vector2(panOffset.GetX() + cursorFromCenter.x * (1.0f - zoomRatio), panOffset.GetY() + cursorFromCenter.y * (1.0f - zoomRatio));
			textureEditorTab->SetPanOffset(panOffset);
		}

		const float zoomFactor = textureEditorTab->GetZoomFactor();
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
		materialInstance->SetVec2("ubo.checkerScale", Vector2(width / (4 * 8), height / (4 * 8))); // 16 pixels squares, whatever the zoom
		materialInstance->SetVec2("ubo.textureSize", Vector2((float)texture->GetWidth(), (float)texture->GetHeight()));
		materialInstance->SetFloat("ubo.pixelsPerTexel", zoomFactor);
		// Texel grid: fades in between 6 and 12 pixels per texel, it would hide the texels if smaller
		materialInstance->SetFloat("ubo.gridOpacity", Clamp((zoomFactor - 6.0f) / 6.0f, 0.0f, 1.0f));
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
