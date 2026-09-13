#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/UI2/Node.hpp>

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>

#include <algorithm>
#include <array>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	UIPrefabEditorViewportWindow::UIPrefabEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Viewport");

		_renderTarget = Renderer::GetInstance()->CreateRenderTarget();
	}

	/// @brief
	UIPrefabEditorViewportWindow::~UIPrefabEditorViewportWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief
	void UIPrefabEditorViewportWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         root = tab->GetCanvas().GetRootNode();

		Vector2 canvasSize = root->ComputeSize();
		ImVec2  available = ImGui::GetContentRegionAvail();
		if (available.x <= 1.0f || available.y <= 1.0f || canvasSize.GetX() <= 0.0f || canvasSize.GetY() <= 0.0f)
		{
			return;
		}

		uint32_t resolutionWidth = (uint32_t)available.x;
		uint32_t resolutionHeight = (uint32_t)available.y;

		resolutionWidth = std::clamp(resolutionWidth, 2u, 16u * 1024u);
		resolutionHeight = std::clamp(resolutionHeight, 2u, 16u * 1024u);

		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			Texture::CreateInfo createInfo;

			createInfo._allowReadWrite = false;
			_renderTarget->Init(resolutionWidth, resolutionHeight, createInfo); // todo error
		}

		if (_renderTarget->IsValid() == false)
		{
			return;
		}

		// Uniform scale (pixels per canvas unit) that fits the whole canvas inside the viewport at zoom = 1, letterboxed.
		float baseScale = std::min(resolutionWidth / canvasSize.GetX(), resolutionHeight / canvasSize.GetY());

		bool hovered = ImGui::IsWindowHovered();
		if (hovered && ImGui::GetIO().MouseWheel != 0.0f)
		{
			_zoom = std::clamp(_zoom + ImGui::GetIO().MouseWheel * 0.1f * _zoom, 0.05f, 50.0f);
		}

		float scale = baseScale * _zoom;

		if (hovered && ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
		{
			// Screen space is Y-down; canvas space is Y-up, hence the flip on Y.
			_cameraPosition.SetX(_cameraPosition.GetX() - ImGui::GetIO().MouseDelta.x / scale);
			_cameraPosition.SetY(_cameraPosition.GetY() + ImGui::GetIO().MouseDelta.y / scale);
		}

		RenderView* renderView = Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
		renderView->Init();
		renderView->Prepare(_renderTarget, nullptr);

		Rect viewportRect;
		viewportRect._position.SetX(0);
		viewportRect._position.SetY(0);
		viewportRect._size.SetX((float)resolutionWidth);
		viewportRect._size.SetY((float)resolutionHeight);

		float worldHalfWidth = resolutionWidth * 0.5f / scale;
		float worldHalfHeight = resolutionHeight * 0.5f / scale;

		Matrix4 projection = Matrix4::OrthogonalProjection(-worldHalfWidth, worldHalfWidth, -worldHalfHeight, worldHalfHeight, -1024, 1024);
		Matrix4 view = Matrix4::Translation(_cameraPosition);

		renderView->SetupCamera(projection, view, viewportRect);

		MaterialInstance* backgroundMaterial = Renderer::GetInstance()->CreateMaterialInstance(
			MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		backgroundMaterial->SetVec4("ubo.color", Vector4(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f));

		std::array<Vector2, 4> backgroundVertices = {
			Vector2(-canvasSize.GetX() * 0.5f, canvasSize.GetY() * 0.5f),
			Vector2(canvasSize.GetX() * 0.5f, canvasSize.GetY() * 0.5f),
			Vector2(canvasSize.GetX() * 0.5f, -canvasSize.GetY() * 0.5f),
			Vector2(-canvasSize.GetX() * 0.5f, -canvasSize.GetY() * 0.5f),
		};
		std::array<uint16_t, 6> backgroundIndices = {0, 1, 2, 0, 2, 3};

		RenderCommandMesh* backgroundCommand = DefaultAllocator::GetInstance().New<RenderCommandMesh>(
			backgroundVertices.data(), nullptr, nullptr, (uint32_t)backgroundVertices.size(), backgroundIndices.data(), (uint32_t)backgroundIndices.size(),
			Matrix4::Identity, backgroundMaterial, 0);
		renderView->PushRenderCommand(backgroundCommand);
		renderView->DeleteAfter(backgroundMaterial);

		Gizmos::Rect(Matrix4::Identity, canvasSize, Color(90.0f / 255.0f, 90.0f / 255.0f, 90.0f / 255.0f, 1.0f), *renderView);

		DrawNode(root, *renderView);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - resolutionWidth) * 0.5f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - resolutionHeight) * 0.5f);
		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));

		if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 mouseImagePos = ImGui::GetIO().MousePos - imagePos;
			if (mouseImagePos.x >= 0.0f && mouseImagePos.x < (float)resolutionWidth && mouseImagePos.y >= 0.0f && mouseImagePos.y < (float)resolutionHeight)
			{
				float   ndcX = (2.0f * mouseImagePos.x) / resolutionWidth - 1.0f;
				float   ndcY = 1.0f - (2.0f * mouseImagePos.y) / resolutionHeight;
				// `view` here is the camera's world matrix (RenderView::SetupCamera inverts it internally),
				// so unprojection is view * inverse(projection), not inverse(projection * view).
				Vector4 mouseWorld = view * Matrix4::Inverse(projection) * Vector4(ndcX, ndcY, 0.0f, 1.0f);
				Vector2 mouseCanvasPos(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
				tab->SetSelectedNode(PickNode(root, mouseCanvasPos));
			}
		}
	}

	/// @brief
	/// @param node
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawNode(ui2::Node* node, RenderView& renderView)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		Vector2 size = node->ComputeSize();
		Vector2 canvasPosition = node->ComputeCanvasMatrix().GetTranslation();

		bool  isSelected = (node == tab->GetSelectedNode());
		Color color = isSelected ? Color(1.0f, 165.0f / 255.0f, 0.0f, 1.0f) : Color(120.0f / 255.0f, 170.0f / 255.0f, 1.0f, 180.0f / 255.0f);

		Gizmos::Rect(Matrix4::Translation(canvasPosition), size, color, renderView);

		for (ui2::Node* child : node->GetChildren())
		{
			DrawNode(child, renderView);
		}
	}

	/// @brief
	/// @param node
	/// @param canvasPosition
	/// @return
	ui2::Node* UIPrefabEditorViewportWindow::PickNode(ui2::Node* node, const Vector2& canvasPosition)
	{
		ui2::Node* picked = nullptr;

		Vector2 size = node->ComputeSize();
		Vector2 nodeCanvasPosition = node->ComputeCanvasMatrix().GetTranslation();
		Vector2 halfExtent = size * 0.5f;

		if (canvasPosition.GetX() >= nodeCanvasPosition.GetX() - halfExtent.GetX() && canvasPosition.GetX() <= nodeCanvasPosition.GetX() + halfExtent.GetX() &&
		    canvasPosition.GetY() >= nodeCanvasPosition.GetY() - halfExtent.GetY() && canvasPosition.GetY() <= nodeCanvasPosition.GetY() + halfExtent.GetY())
		{
			picked = node;
		}

		for (ui2::Node* child : node->GetChildren())
		{
			ui2::Node* childPicked = PickNode(child, canvasPosition);
			if (childPicked != nullptr)
			{
				picked = childPicked;
			}
		}

		return picked;
	}
}
