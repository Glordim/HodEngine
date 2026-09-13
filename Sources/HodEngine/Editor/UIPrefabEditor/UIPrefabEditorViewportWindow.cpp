#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

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
#include <cmath>

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
			_targetZoom = std::clamp(_targetZoom + ImGui::GetIO().MouseWheel * 0.1f * _targetZoom, 0.05f, 50.0f);
		}

		// Ease towards the target zoom instead of snapping, so each wheel notch feels smooth.
		_zoom += (_targetZoom - _zoom) * std::min(1.0f, ImGui::GetIO().DeltaTime * 12.0f);

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

		DrawGrid(_cameraPosition, worldHalfWidth, worldHalfHeight, scale, *renderView);

		Gizmos::Rect(Matrix4::Identity, canvasSize, Color(90.0f / 255.0f, 90.0f / 255.0f, 90.0f / 255.0f, 1.0f), *renderView);

		DrawNode(root, *renderView);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - resolutionWidth) * 0.5f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - resolutionHeight) * 0.5f);
		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));

		// `view` here is the camera's world matrix (RenderView::SetupCamera inverts it internally),
		// so unprojection is view * inverse(projection), not inverse(projection * view).
		Matrix4 inverseProjection = Matrix4::Inverse(projection);
		auto    screenToCanvas = [&](const ImVec2& screenPos) -> Vector2
		{
			float   ndcX = (2.0f * (screenPos.x - imagePos.x)) / resolutionWidth - 1.0f;
			float   ndcY = 1.0f - (2.0f * (screenPos.y - imagePos.y)) / resolutionHeight;
			Vector4 world = view * inverseProjection * Vector4(ndcX, ndcY, 0.0f, 1.0f);
			return Vector2(world.GetX() / world.GetW(), world.GetY() / world.GetW());
		};

		if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 mouseImagePos = ImGui::GetIO().MousePos - imagePos;
			if (mouseImagePos.x >= 0.0f && mouseImagePos.x < (float)resolutionWidth && mouseImagePos.y >= 0.0f && mouseImagePos.y < (float)resolutionHeight)
			{
				tab->SetSelectedNode(PickNode(root, screenToCanvas(ImGui::GetIO().MousePos)));
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI2NodeLibraryDescriptor");
			if (payload != nullptr)
			{
				ReflectionDescriptor* nodeDescriptor = *static_cast<ReflectionDescriptor**>(payload->Data);

				ui2::Node* parent = tab->GetSelectedNode();
				if (parent == nullptr)
				{
					parent = root;
				}

				Vector2 dropCanvasPos = screenToCanvas(ImGui::GetIO().MousePos);
				Vector2 parentCanvasPos = parent->ComputeCanvasMatrix().GetTranslation();
				// Parent rotation/scale ignored when converting to a local offset, same simplification as DrawNode/PickNode.
				Vector2 localOffset = dropCanvasPos - parentCanvasPos;

				ui2::Node*    newNode = nodeDescriptor->CreateInstance<ui2::Node>();
				LayoutParams* layoutParams = parent->CreateDefaultLayoutParams();
				if (AnchoredLayoutParams* anchoredLayoutParams = dynamic_cast<AnchoredLayoutParams*>(layoutParams))
				{
					anchoredLayoutParams->SetOffset(localOffset);
				}
				parent->AddChild(newNode, layoutParams);

				tab->SetSelectedNode(newNode);
				tab->MarkAsDirty();
			}
			ImGui::EndDragDropTarget();
		}
	}

	/// @brief Draws an infinite-looking grid covering the whole visible area (not just the canvas),
	/// with a world-space step that adapts to zoom so the on-screen spacing stays readable.
	///
	/// The finer subdivision (half the major step) is pre-drawn and progressively faded in as the
	/// zoom approaches the point where it would become the new major step, instead of the grid
	/// popping to a denser level all at once.
	/// @param viewCenter
	/// @param worldHalfWidth
	/// @param worldHalfHeight
	/// @param scale pixels per world/canvas unit
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawGrid(const Vector2& viewCenter, float worldHalfWidth, float worldHalfHeight, float scale, RenderView& renderView)
	{
		constexpr float targetScreenSpacing = 64.0f;

		float desiredStep = targetScreenSpacing / scale;
		float level = std::log2(std::max(desiredStep, 1e-6f));
		float majorLevel = std::ceil(level);
		float majorStep = std::pow(2.0f, majorLevel);
		float minorStep = majorStep * 0.5f;

		// 0 right after a level switch, ramping up to 1 as the minor grid is about to become the new major grid.
		float minorFade = majorLevel - level;

		// These commands aren't GPU-blended (they overwrite, not blend), so "fading in" the minor grid
		// is done by lerping its color towards the background instead of lowering its alpha.
		Color backgroundColor(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f);
		Color gridColor(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
		Color minorColor(backgroundColor.r + (gridColor.r - backgroundColor.r) * minorFade, backgroundColor.g + (gridColor.g - backgroundColor.g) * minorFade,
		                  backgroundColor.b + (gridColor.b - backgroundColor.b) * minorFade, 1.0f);

		float left = viewCenter.GetX() - worldHalfWidth;
		float right = viewCenter.GetX() + worldHalfWidth;
		float bottom = viewCenter.GetY() - worldHalfHeight;
		float top = viewCenter.GetY() + worldHalfHeight;

		// Minor grid first (every half-step, skipping the ones that coincide with a major line), then major on top.
		int64_t firstMinorX = (int64_t)std::floor(left / minorStep);
		int64_t lastMinorX = (int64_t)std::ceil(right / minorStep);
		for (int64_t i = firstMinorX; i <= lastMinorX; ++i)
		{
			if (i % 2 != 0)
			{
				float x = (float)i * minorStep;
				Gizmos::Line(Matrix4::Identity, Vector2(x, bottom), Vector2(x, top), minorColor, renderView);
			}
		}

		int64_t firstMinorY = (int64_t)std::floor(bottom / minorStep);
		int64_t lastMinorY = (int64_t)std::ceil(top / minorStep);
		for (int64_t i = firstMinorY; i <= lastMinorY; ++i)
		{
			if (i % 2 != 0)
			{
				float y = (float)i * minorStep;
				Gizmos::Line(Matrix4::Identity, Vector2(left, y), Vector2(right, y), minorColor, renderView);
			}
		}

		float firstX = std::floor(left / majorStep) * majorStep;
		for (float x = firstX; x <= right; x += majorStep)
		{
			Gizmos::Line(Matrix4::Identity, Vector2(x, bottom), Vector2(x, top), gridColor, renderView);
		}

		float firstY = std::floor(bottom / majorStep) * majorStep;
		for (float y = firstY; y <= top; y += majorStep)
		{
			Gizmos::Line(Matrix4::Identity, Vector2(left, y), Vector2(right, y), gridColor, renderView);
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
