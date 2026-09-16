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

		tab->GetCanvas().UpdateLayout();
		Vector2 canvasSize = root->GetSize();
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

		// Captured here (before any further ImGui:: layout calls) so screenToCanvas below can be
		// used both for gizmo interaction (computed before the RHI commands that draw the gizmo)
		// and for the click-to-select / drag-drop handling after ImGui::Image is issued.
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - resolutionWidth) * 0.5f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - resolutionHeight) * 0.5f);
		ImVec2 imagePos = ImGui::GetCursorScreenPos();

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

		ImVec2 mouseImagePos = ImGui::GetIO().MousePos - imagePos;
		bool   mouseInsideImage =
			mouseImagePos.x >= 0.0f && mouseImagePos.x < (float)resolutionWidth && mouseImagePos.y >= 0.0f && mouseImagePos.y < (float)resolutionHeight;
		Vector2 mouseCanvasPos = screenToCanvas(ImGui::GetIO().MousePos);

		ui2::Node* selectedNode = tab->GetSelectedNode();
		if (selectedNode != nullptr)
		{
			UpdateGizmoDrag(selectedNode, mouseCanvasPos, scale, hovered && mouseInsideImage);
		}
		else
		{
			_hoveredGizmoHandle = GizmoHandle::None;
			_draggedGizmoHandle = GizmoHandle::None;
		}

		GizmoHandle activeGizmoHandle = _draggedGizmoHandle != GizmoHandle::None ? _draggedGizmoHandle : _hoveredGizmoHandle;
		switch (activeGizmoHandle)
		{
			case GizmoHandle::Move: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll); break;
			case GizmoHandle::Top:
			case GizmoHandle::Bottom: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS); break;
			case GizmoHandle::Left:
			case GizmoHandle::Right: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); break;
			case GizmoHandle::TopLeft:
			case GizmoHandle::BottomRight: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE); break;
			case GizmoHandle::TopRight:
			case GizmoHandle::BottomLeft: ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW); break;
			default: break;
		}

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

		if (selectedNode != nullptr && LayoutParams::Cast<AnchoredLayoutParams>(selectedNode->GetLayoutParams()) != nullptr)
		{
			DrawSelectionGizmo(selectedNode, scale, *renderView);
		}

		ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));

		if (hovered && mouseInsideImage && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && _draggedGizmoHandle == GizmoHandle::None)
		{
			tab->SetSelectedNode(PickNode(root, mouseCanvasPos));
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
				if (AnchoredLayoutParams* anchoredLayoutParams = LayoutParams::Cast<AnchoredLayoutParams>(layoutParams))
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

		Vector2 size = node->GetSize();
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

		Vector2 size = node->GetSize();
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

	namespace
	{
		constexpr float gizmoCornerHandlePixelSize = 8.0f;
		constexpr float gizmoEdgeHandlePixelThickness = 6.0f;

		/// @brief Filled quad in canvas space, used to draw the corner handles (Gizmos::Rect only
		/// draws an outline, which would be hard to see at handle size).
		void DrawFilledQuad(const Vector2& center, const Vector2& size, const Color& color, RenderView& renderView)
		{
			Vector2                halfSize = size * 0.5f;
			std::array<Vector2, 4> vertices = {
				center + Vector2(-halfSize.GetX(), halfSize.GetY()),
				center + Vector2(halfSize.GetX(), halfSize.GetY()),
				center + Vector2(halfSize.GetX(), -halfSize.GetY()),
				center + Vector2(-halfSize.GetX(), -halfSize.GetY()),
			};
			std::array<uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};

			MaterialInstance* materialInstance = Renderer::GetInstance()->CreateMaterialInstance(
				MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
			materialInstance->SetVec4("ubo.color", Vector4(color.r, color.g, color.b, color.a));

			RenderCommandMesh* command = DefaultAllocator::GetInstance().New<RenderCommandMesh>(
				vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), Matrix4::Identity, materialInstance,
				std::numeric_limits<uint32_t>::max());
			renderView.PushRenderCommand(command);
			renderView.DeleteAfter(materialInstance);
		}
	}

	/// @brief Draws the move/resize handles over the selected node: a filled square at each of the
	/// 4 corners (resize keeping the opposite corner fixed) and a highlighted edge line when
	/// hovering/dragging one of the 4 edges (resize along one axis only). The node's body itself
	/// (already drawn as an outline by DrawNode) acts as the move handle.
	/// Mirrors UIEditor's NodeCustomComponentDrawer::OnDrawGizmo, minus rotation support (this
	/// viewport draws every node axis-aligned, see the class comment on the header).
	/// @param node
	/// @param scale pixels per canvas unit, used to keep the handle size constant on screen
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawSelectionGizmo(ui2::Node* node, float scale, RenderView& renderView)
	{
		Vector2 size = node->GetSize();
		Vector2 canvasPosition = node->ComputeCanvasMatrix().GetTranslation();
		Vector2 halfSize = size * 0.5f;

		float cornerSize = gizmoCornerHandlePixelSize / scale;

		Color handleColor(0.25f, 0.25f, 1.0f, 1.0f);
		Color handleHighlightColor(0.5f, 0.5f, 1.0f, 1.0f);

		Vector2 topLeft = canvasPosition + Vector2(-halfSize.GetX(), halfSize.GetY());
		Vector2 topRight = canvasPosition + Vector2(halfSize.GetX(), halfSize.GetY());
		Vector2 bottomLeft = canvasPosition + Vector2(-halfSize.GetX(), -halfSize.GetY());
		Vector2 bottomRight = canvasPosition + Vector2(halfSize.GetX(), -halfSize.GetY());

		GizmoHandle activeHandle = _draggedGizmoHandle != GizmoHandle::None ? _draggedGizmoHandle : _hoveredGizmoHandle;

		auto isActive = [&](GizmoHandle handle) { return activeHandle == handle; };

		DrawFilledQuad(topLeft, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::TopLeft) ? handleHighlightColor : handleColor, renderView);
		DrawFilledQuad(topRight, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::TopRight) ? handleHighlightColor : handleColor, renderView);
		DrawFilledQuad(bottomLeft, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::BottomLeft) ? handleHighlightColor : handleColor, renderView);
		DrawFilledQuad(bottomRight, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::BottomRight) ? handleHighlightColor : handleColor, renderView);

		if (isActive(GizmoHandle::Top))
		{
			Gizmos::Line(Matrix4::Identity, topLeft, topRight, handleHighlightColor, renderView);
		}
		if (isActive(GizmoHandle::Bottom))
		{
			Gizmos::Line(Matrix4::Identity, bottomLeft, bottomRight, handleHighlightColor, renderView);
		}
		if (isActive(GizmoHandle::Left))
		{
			Gizmos::Line(Matrix4::Identity, topLeft, bottomLeft, handleHighlightColor, renderView);
		}
		if (isActive(GizmoHandle::Right))
		{
			Gizmos::Line(Matrix4::Identity, topRight, bottomRight, handleHighlightColor, renderView);
		}
	}

	/// @brief CPU hit-test of the gizmo handles against a canvas-space mouse position (this
	/// viewport has no RHI picking texture, unlike ViewportWindow/Gizmos::FreeMoveRect). Corners
	/// are tested first so their (small) area takes priority over the edge bands that reach them.
	/// @param node
	/// @param canvasMousePosition
	/// @param scale pixels per canvas unit
	/// @return
	UIPrefabEditorViewportWindow::GizmoHandle UIPrefabEditorViewportWindow::HitTestGizmoHandle(ui2::Node* node, const Vector2& canvasMousePosition, float scale) const
	{
		Vector2 size = node->GetSize();
		Vector2 canvasPosition = node->ComputeCanvasMatrix().GetTranslation();
		Vector2 halfSize = size * 0.5f;

		float cornerRadius = gizmoCornerHandlePixelSize * 0.5f / scale;
		float edgeThickness = gizmoEdgeHandlePixelThickness * 0.5f / scale;

		Vector2 topLeft = canvasPosition + Vector2(-halfSize.GetX(), halfSize.GetY());
		Vector2 topRight = canvasPosition + Vector2(halfSize.GetX(), halfSize.GetY());
		Vector2 bottomLeft = canvasPosition + Vector2(-halfSize.GetX(), -halfSize.GetY());
		Vector2 bottomRight = canvasPosition + Vector2(halfSize.GetX(), -halfSize.GetY());

		auto withinCorner = [&](const Vector2& corner)
		{
			return std::abs(canvasMousePosition.GetX() - corner.GetX()) <= cornerRadius && std::abs(canvasMousePosition.GetY() - corner.GetY()) <= cornerRadius;
		};

		if (withinCorner(topLeft))
		{
			return GizmoHandle::TopLeft;
		}
		if (withinCorner(topRight))
		{
			return GizmoHandle::TopRight;
		}
		if (withinCorner(bottomLeft))
		{
			return GizmoHandle::BottomLeft;
		}
		if (withinCorner(bottomRight))
		{
			return GizmoHandle::BottomRight;
		}

		bool withinX = canvasMousePosition.GetX() >= topLeft.GetX() && canvasMousePosition.GetX() <= topRight.GetX();
		bool withinY = canvasMousePosition.GetY() >= bottomLeft.GetY() && canvasMousePosition.GetY() <= topLeft.GetY();

		if (withinX && std::abs(canvasMousePosition.GetY() - topLeft.GetY()) <= edgeThickness)
		{
			return GizmoHandle::Top;
		}
		if (withinX && std::abs(canvasMousePosition.GetY() - bottomLeft.GetY()) <= edgeThickness)
		{
			return GizmoHandle::Bottom;
		}
		if (withinY && std::abs(canvasMousePosition.GetX() - topLeft.GetX()) <= edgeThickness)
		{
			return GizmoHandle::Left;
		}
		if (withinY && std::abs(canvasMousePosition.GetX() - topRight.GetX()) <= edgeThickness)
		{
			return GizmoHandle::Right;
		}

		if (withinX && withinY)
		{
			return GizmoHandle::Move;
		}

		return GizmoHandle::None;
	}

	/// @brief Drives the move/resize drag state machine and, while dragging, writes the result
	/// straight to the node's AnchoredLayoutParams offset / Node desired size. Same math as
	/// NodeCustomComponentDrawer::OnDrawGizmo (Position/DeltaSize there map 1:1 to
	/// AnchoredLayoutParams::Offset/Node::DesiredSize here, see AnchoredLayoutParams::ComputePosition
	/// vs. the old UI Node::ComputeLocalMatrix), minus the old code's leftover "* 100.0f" unit hack:
	/// this viewport's canvas-space mouse delta is already in the same units as the node's own size.
	/// @param node
	/// @param canvasMousePosition
	/// @param scale pixels per canvas unit
	/// @param mouseAvailable whether the mouse is over this viewport's image and usable for a new hit-test
	void UIPrefabEditorViewportWindow::UpdateGizmoDrag(ui2::Node* node, const Vector2& canvasMousePosition, float scale, bool mouseAvailable)
	{
		AnchoredLayoutParams* layoutParams = LayoutParams::Cast<AnchoredLayoutParams>(node->GetLayoutParams());
		if (layoutParams == nullptr)
		{
			_hoveredGizmoHandle = GizmoHandle::None;
			_draggedGizmoHandle = GizmoHandle::None;
			return;
		}

		if (_draggedGizmoHandle != GizmoHandle::None)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
			{
				_draggedGizmoHandle = GizmoHandle::None;
				return;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				layoutParams->SetOffset(_gizmoDragStartOffset);
				node->SetDesiredSize(_gizmoDragStartDesiredSize);
				_draggedGizmoHandle = GizmoHandle::None;
				GetOwner()->MarkAsDirty();
				return;
			}

			Vector2 delta = canvasMousePosition - _gizmoDragStartMousePosition;

			if (_draggedGizmoHandle == GizmoHandle::Move)
			{
				layoutParams->SetOffset(_gizmoDragStartOffset + delta);
			}
			else
			{
				Vector2 pivot = layoutParams->GetPivot();
				Vector2 sizeDelta = Vector2::Zero;
				Vector2 reverse = Vector2::Zero;

				switch (_draggedGizmoHandle)
				{
					case GizmoHandle::Top:
						sizeDelta = Vector2(0.0f, delta.GetY());
						reverse = Vector2(0.0f, 0.0f);
						break;
					case GizmoHandle::Bottom:
						sizeDelta = Vector2(0.0f, -delta.GetY());
						reverse = Vector2(0.0f, 1.0f);
						break;
					case GizmoHandle::Left:
						sizeDelta = Vector2(-delta.GetX(), 0.0f);
						reverse = Vector2(1.0f, 0.0f);
						break;
					case GizmoHandle::Right:
						sizeDelta = Vector2(delta.GetX(), 0.0f);
						reverse = Vector2(0.0f, 0.0f);
						break;
					case GizmoHandle::TopLeft:
						sizeDelta = Vector2(-delta.GetX(), delta.GetY());
						reverse = Vector2(1.0f, 0.0f);
						break;
					case GizmoHandle::TopRight:
						sizeDelta = Vector2(delta.GetX(), delta.GetY());
						reverse = Vector2(0.0f, 0.0f);
						break;
					case GizmoHandle::BottomLeft:
						sizeDelta = Vector2(-delta.GetX(), -delta.GetY());
						reverse = Vector2(1.0f, 1.0f);
						break;
					case GizmoHandle::BottomRight:
						sizeDelta = Vector2(delta.GetX(), -delta.GetY());
						reverse = Vector2(0.0f, 1.0f);
						break;
					default: break;
				}

				Vector2 newDesiredSize = _gizmoDragStartDesiredSize + sizeDelta;
				layoutParams->SetOffset(_gizmoDragStartOffset - sizeDelta * (reverse - pivot));
				node->SetDesiredSize(newDesiredSize);
			}

			GetOwner()->MarkAsDirty();
			return;
		}

		if (mouseAvailable == false)
		{
			_hoveredGizmoHandle = GizmoHandle::None;
			return;
		}

		_hoveredGizmoHandle = HitTestGizmoHandle(node, canvasMousePosition, scale);

		if (_hoveredGizmoHandle != GizmoHandle::None && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			_draggedGizmoHandle = _hoveredGizmoHandle;
			_gizmoDragStartMousePosition = canvasMousePosition;
			_gizmoDragStartOffset = layoutParams->GetOffset();
			_gizmoDragStartDesiredSize = node->GetDesiredSize();
		}
	}
}
