#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

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
#include <vector>

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

		_settings.Load();

		_renderTarget = Renderer::GetInstance()->CreateRenderTarget();
	}

	/// @brief
	UIPrefabEditorViewportWindow::~UIPrefabEditorViewportWindow()
	{
		if (_settingsDirty)
		{
			_settings.Save();
		}

		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief
	void UIPrefabEditorViewportWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		ui2::Node*         root = tab->GetCanvas().GetRootNode();

		DrawToolbar();

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

		/*
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
*/
		// The grid and all the guides share the canvas' top-left corner as origin (the canvas rect is
		// centered on the canvas origin), so a guide's edges always fall on grid lines.
		Vector2 gridOrigin(-canvasSize.GetX() * 0.5f, canvasSize.GetY() * 0.5f);
		if (_settings._gridVisible)
		{
			DrawGrid(gridOrigin, _cameraPosition, worldHalfWidth, worldHalfHeight, scale, *renderView);
		}
		DrawGuides(gridOrigin, *renderView);

		//Gizmos::Rect(Matrix4::Identity, canvasSize, Color(90.0f / 255.0f, 90.0f / 255.0f, 90.0f / 255.0f, 1.0f), *renderView);

		// The actual content first: the outlines/gizmos below are pushed with a later (or the maximum)
		// rendering order, so they stay on top of it.
		tab->GetCanvas().PushRenderCommand(*renderView, RenderView::RenderQueueType::World);

		// Only the prefab root's subtree: `root` is the hidden container, whose rect is the design resolution, not part of the prefab.
		DrawNode(tab->GetPrefabRoot(), *renderView);

		if (selectedNode != nullptr && LayoutParams::Cast<AnchoredLayoutParams>(selectedNode->GetLayoutParams()) != nullptr)
		{
			DrawSelectionGizmo(selectedNode, scale, *renderView);
		}

		ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));
		DrawGuideLabels(gridOrigin, imagePos, ImVec2((float)resolutionWidth, (float)resolutionHeight), scale);

		if (hovered && mouseInsideImage && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && _draggedGizmoHandle == GizmoHandle::None)
		{
			tab->SetSelectedNode(PickNode(tab->GetPrefabRoot(), mouseCanvasPos));
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
					parent = tab->GetPrefabRoot();
				}

				Vector2 dropCanvasPos = screenToCanvas(ImGui::GetIO().MousePos);
				Vector2 parentCanvasPos = parent->ComputeCanvasMatrix().GetTranslation();
				// Parent rotation/scale ignored when converting to a local offset, same simplification as DrawNode/PickNode.
				Vector2 localOffset = dropCanvasPos - parentCanvasPos;

				ui2::Node*    newNode = ui2::NodeFactory::GetInstance()->CreateNode(*nodeDescriptor);
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

	/// @brief Toolbar on top of the viewport: a "Guides" and a "Grid" toggle, each glued to a small
	/// arrow button opening its settings (see DrawHelper::DrawToggleWithSettings).
	void UIPrefabEditorViewportWindow::DrawToolbar()
	{
		_settingsDirty |= DrawHelper::DrawToggleWithSettings(ICON_MDI_RULER_SQUARE " Guides", "guides", "UIPrefabGuides", _settings._guidesVisible);
		ImGui::SameLine();
		_settingsDirty |= DrawHelper::DrawToggleWithSettings(ICON_MDI_GRID " Grid", "grid", "UIPrefabGrid", _settings._gridVisible);

		DrawGuidesSettingsPopup();
		DrawGridSettingsPopup();

		// Saved once the user is done editing (not on every keystroke/drag step): as soon as no widget is active anymore.
		if (_settingsDirty && ImGui::IsAnyItemActive() == false)
		{
			_settings.Save();
			_settingsDirty = false;
		}

		ImGui::Separator();
	}

	/// @brief Editable list of guides: one row per guide (enabled checkbox, color, name,
	/// resolution, delete) plus an "Add Guide" button.
	void UIPrefabEditorViewportWindow::DrawGuidesSettingsPopup()
	{
		if (DrawHelper::BeginSettingsPopup("UIPrefabGuides"))
		{
			int32_t removeIndex = -1;
			for (uint32_t i = 0; i < _settings._guides.Size(); ++i)
			{
				Guide& guide = _settings._guides[i];

				ImGui::PushID((int)i);

				_settingsDirty |= ImGui::Checkbox("##Enabled", &guide._enabled);

				ImGui::SameLine();
				float color[3] = {guide._color.r, guide._color.g, guide._color.b};
				if (ImGui::ColorEdit3("##Color", color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
				{
					guide._color = Color(color[0], color[1], color[2], 1.0f);
					_settingsDirty = true;
				}

				ImGui::SameLine();
				char nameBuffer[128] = {'\0'};
				std::strncpy(nameBuffer, guide._name.CStr(), sizeof(nameBuffer) - 1);
				ImGui::SetNextItemWidth(140.0f);
				if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
				{
					guide._name = nameBuffer;
					_settingsDirty = true;
				}

				ImGui::SameLine();
				int32_t resolution[2] = {(int32_t)guide._resolution.GetX(), (int32_t)guide._resolution.GetY()};
				ImGui::SetNextItemWidth(170.0f);
				if (ImGui::InputInt2("##Resolution", resolution))
				{
					guide._resolution = Vector2((float)std::max(resolution[0], 1), (float)std::max(resolution[1], 1));
					_settingsDirty = true;
				}

				ImGui::SameLine();
				if (ImGui::Button(ICON_MDI_DELETE))
				{
					removeIndex = (int32_t)i;
				}

				ImGui::PopID();
			}

			if (removeIndex >= 0)
			{
				_settings._guides.Erase((uint32_t)removeIndex);
				_settingsDirty = true;
			}

			if (_settings._guides.Empty())
			{
				ImGui::TextDisabled("No guide");
			}

			if (ImGui::Button(ICON_MDI_PLUS " Add Guide"))
			{
				static const std::array<Color, 5> palette = {
					Color(1.0f, 0.35f, 0.35f, 1.0f), Color(0.35f, 1.0f, 0.45f, 1.0f), Color(0.35f, 0.65f, 1.0f, 1.0f),
					Color(1.0f, 0.85f, 0.3f, 1.0f),  Color(0.85f, 0.45f, 1.0f, 1.0f),
				};

				Guide& guide = _settings._guides.EmplaceBack();
				guide._name = String("Guide ") + String(std::to_string(_settings._guides.Size()).c_str());
				guide._color = palette[(_settings._guides.Size() - 1) % palette.size()];
				_settingsDirty = true;
			}

			ImGui::EndPopup();
		}
	}

	/// @brief Grid settings: size of a cell at zoom 1.0 (the grid still coarsens/refines by
	/// powers of two as the zoom changes, see DrawGrid).
	void UIPrefabEditorViewportWindow::DrawGridSettingsPopup()
	{
		if (DrawHelper::BeginSettingsPopup("UIPrefabGrid"))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Cell size");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(120.0f);
			_settingsDirty |= ImGui::DragFloat("##CellSize", &_settings._gridCellSize, 1.0f, 1.0f, 100000.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndPopup();
		}
	}

	/// @brief Enabled guides (none at all while guides are hidden) ordered from the largest to the smallest area, i.e. the order they
	/// must be drawn in so that a smaller guide's frame ends up on top of a larger one's and stays
	/// complete where their edges coincide. The user-facing list order is left untouched.
	std::vector<const UIPrefabEditorViewportWindow::Guide*> UIPrefabEditorViewportWindow::GetGuidesToDraw() const
	{
		std::vector<const Guide*> guides;
		if (_settings._guidesVisible == false)
		{
			return guides;
		}

		for (const Guide& guide : _settings._guides)
		{
			if (guide._enabled)
			{
				guides.push_back(&guide);
			}
		}

		std::stable_sort(guides.begin(), guides.end(),
		                 [](const Guide* left, const Guide* right)
		                 { return left->_resolution.GetX() * left->_resolution.GetY() > right->_resolution.GetX() * right->_resolution.GetY(); });
		return guides;
	}

	/// @brief Draws each enabled guide's rect into the render target, all sharing the same top-left
	/// corner (`origin`) and extending right/down from it, largest first (see GetGuidesToDraw).
	/// Their labels are drawn afterwards over the blitted image, see DrawGuideLabels.
	/// @param origin canvas-space position of the guides' shared top-left corner
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawGuides(const Vector2& origin, RenderView& renderView)
	{
		for (const Guide* guide : GetGuidesToDraw())
		{
			// Gizmos::Rect is centered on its matrix; canvas space is Y-up, so "down" is -Y.
			Vector2 center(origin.GetX() + guide->_resolution.GetX() * 0.5f, origin.GetY() - guide->_resolution.GetY() * 0.5f);
			Gizmos::Rect(Matrix4::Translation(center), guide->_resolution, guide->_color, renderView);
		}
	}

	/// @brief Draws each enabled guide's name just inside the bottom-right corner of its rect.
	/// Done with ImGui over the image (the render target can't draw text), hence the manual
	/// canvas -> screen projection, the inverse of DrawContent's screenToCanvas.
	/// @param origin canvas-space position of the guides' shared top-left corner, see DrawGuides
	/// @param imagePos screen position of the image's top-left corner
	/// @param imageSize
	/// @param scale pixels per canvas unit
	void UIPrefabEditorViewportWindow::DrawGuideLabels(const Vector2& origin, const ImVec2& imagePos, const ImVec2& imageSize, float scale)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(imagePos, imagePos + imageSize, true);

		for (const Guide* guidePtr : GetGuidesToDraw())
		{
			const Guide& guide = *guidePtr;
			if (guide._name.Empty())
			{
				continue;
			}

			// Canvas space is Y-up, screen space Y-down; the canvas point at the view's center is _cameraPosition.
			float  right = imagePos.x + imageSize.x * 0.5f + (origin.GetX() + guide._resolution.GetX() - _cameraPosition.GetX()) * scale;
			float  bottom = imagePos.y + imageSize.y * 0.5f - (origin.GetY() - guide._resolution.GetY() - _cameraPosition.GetY()) * scale;
			ImVec2 textSize = ImGui::CalcTextSize(guide._name.CStr());

			ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(guide._color.r, guide._color.g, guide._color.b, 1.0f));
			drawList->AddText(ImVec2(right - textSize.x - 4.0f, bottom - textSize.y - 2.0f), color, guide._name.CStr());
		}

		drawList->PopClipRect();
	}

	/// @brief Draws an infinite-looking grid covering the whole visible area (not just the canvas),
	/// with a world-space step that adapts to zoom so the on-screen spacing stays readable.
	///
	/// At zoom 1.0 the major step is exactly _settings._gridCellSize; zooming out doubles it each time the zoom
	/// halves (and zooming in halves it), so the on-screen spacing stays within [1, 2) times its
	/// zoom-1.0 value. It is additionally kept above a minimum on-screen spacing so a tiny cell
	/// size can't flood the view with lines.
	///
	/// The finer subdivision (half the major step) is pre-drawn and progressively faded in as the
	/// zoom approaches the point where it would become the new major step, instead of the grid
	/// popping to a denser level all at once.
	/// @param origin canvas-space position the grid lines are aligned on (a line always passes through it)
	/// @param viewCenter
	/// @param worldHalfWidth
	/// @param worldHalfHeight
	/// @param scale pixels per world/canvas unit
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawGrid(const Vector2& origin, const Vector2& viewCenter, float worldHalfWidth, float worldHalfHeight, float scale, RenderView& renderView)
	{
		constexpr float minScreenSpacing = 16.0f;

		// Lowest rendering order: the grid is always behind everything else (the canvas content starts at order 1).
		constexpr uint32_t gridRenderingOrder = 0;

		float cellSize = std::max(_settings._gridCellSize, 1e-3f);

		// Level = number of doublings of the cell size: 0 at zoom 1.0, +1 each time the zoom halves.
		float zoomLevel = -std::log2(std::max(_zoom, 1e-6f));
		float minLevel = std::log2(minScreenSpacing / (cellSize * scale));
		float level = std::max(zoomLevel, minLevel);
		float majorLevel = std::ceil(level);
		float majorStep = cellSize * std::pow(2.0f, majorLevel);
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

		// Line positions are indexed relative to `origin` (line i sits at origin + i * step), so the
		// grid is anchored on it rather than on the canvas origin.
		float relLeft = left - origin.GetX();
		float relRight = right - origin.GetX();
		float relBottom = bottom - origin.GetY();
		float relTop = top - origin.GetY();

		// Minor grid first (every half-step, skipping the ones that coincide with a major line), then major on top.
		int64_t firstMinorX = (int64_t)std::floor(relLeft / minorStep);
		int64_t lastMinorX = (int64_t)std::ceil(relRight / minorStep);
		for (int64_t i = firstMinorX; i <= lastMinorX; ++i)
		{
			if (i % 2 != 0)
			{
				float x = origin.GetX() + (float)i * minorStep;
				Gizmos::Line(Matrix4::Identity, Vector2(x, bottom), Vector2(x, top), minorColor, renderView, gridRenderingOrder);
			}
		}

		int64_t firstMinorY = (int64_t)std::floor(relBottom / minorStep);
		int64_t lastMinorY = (int64_t)std::ceil(relTop / minorStep);
		for (int64_t i = firstMinorY; i <= lastMinorY; ++i)
		{
			if (i % 2 != 0)
			{
				float y = origin.GetY() + (float)i * minorStep;
				Gizmos::Line(Matrix4::Identity, Vector2(left, y), Vector2(right, y), minorColor, renderView, gridRenderingOrder);
			}
		}

		int64_t firstMajorX = (int64_t)std::floor(relLeft / majorStep);
		int64_t lastMajorX = (int64_t)std::ceil(relRight / majorStep);
		for (int64_t i = firstMajorX; i <= lastMajorX; ++i)
		{
			float x = origin.GetX() + (float)i * majorStep;
			Gizmos::Line(Matrix4::Identity, Vector2(x, bottom), Vector2(x, top), gridColor, renderView, gridRenderingOrder);
		}

		int64_t firstMajorY = (int64_t)std::floor(relBottom / majorStep);
		int64_t lastMajorY = (int64_t)std::ceil(relTop / majorStep);
		for (int64_t i = firstMajorY; i <= lastMajorY; ++i)
		{
			float y = origin.GetY() + (float)i * majorStep;
			Gizmos::Line(Matrix4::Identity, Vector2(left, y), Vector2(right, y), gridColor, renderView, gridRenderingOrder);
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
