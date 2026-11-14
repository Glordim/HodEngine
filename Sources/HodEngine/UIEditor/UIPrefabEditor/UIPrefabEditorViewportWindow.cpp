#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include "HodEngine/Editor/Gizmos/ViewportGrid.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Widgets.hpp>

#include <HodEngine/Core/Math.hpp>

#include <HodEngine/UI/AnchoredLayoutParams.hpp>
#include <HodEngine/UI/BoxLayout.hpp>
#include <HodEngine/UI/BoxLayoutParams.hpp>
#include <HodEngine/UI/Selectable.hpp>
#include <HodEngine/UI/AnimationClip.hpp>
#include <HodEngine/UI/Canvas.hpp>
#include <HodEngine/UI/LayoutParams.hpp>
#include <HodEngine/UI/Node.hpp>
#include <HodEngine/UI/NodeFactory.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/Texture.hpp>

#include <algorithm>
#include "HodEngine/Core/StaticArray.hpp"
#include <cmath>
#include <vector>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportWindow, reflectionDescriptor) {}

	/// @brief
	UIPrefabEditorViewportWindow::UIPrefabEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Viewport");

		_settings.Load();

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
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
	bool UIPrefabEditorViewportWindow::Draw()
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
	void UIPrefabEditorViewportWindow::DrawContent()
	{
		// The toolbar floats over the render, which fills the whole window. It is submitted first so the viewport
		// knows when the mouse is on its buttons (no picking/zoom/pan through them), but drawn on a channel above the image.
		ImVec2      contentPos = ImGui::GetCursorPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->ChannelsSplit(2);
		drawList->ChannelsSetCurrent(1);
		DrawToolbar();
		bool mouseOverToolbar = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive();
		drawList->ChannelsSetCurrent(0);

		ImGui::SetCursorPos(contentPos);
		DrawViewport(mouseOverToolbar);

		drawList->ChannelsMerge();
	}

	/// @brief
	/// @param mouseOverToolbar true when the mouse is used by the floating toolbar, the viewport then ignores it
	void UIPrefabEditorViewportWindow::DrawViewport(bool mouseOverToolbar)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		// F5 plays, Escape stops. Before the displayed canvas is read below, as it switches with the play mode.
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().WantTextInput == false)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_F5, false))
			{
				tab->Play();
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && _draggedGizmoHandle == GizmoHandle::None && _marqueeState == MarqueeState::None)
			{
				tab->Stop();
			}
		}

		// While playing, the play mode's copy (see UIPrefabEditorTab::Play): laid out, drawn and fed the pointer instead.
		bool         playing = tab->IsPlaying();
		ui::Canvas& canvas = tab->GetDisplayedCanvas();

		// Once when the play mode starts (from the menu bar or F5): the prefab shown as in game, the preview can still be turned
		// off meanwhile. Once when it stops: the preview set back as before, not saved as on.
		if (playing != _wasPlaying)
		{
			_wasPlaying = playing;
			if (playing)
			{
				_previewBeforePlay = _settings._preview;
				_settings._preview = true;
			}
			else if (_settings._preview != _previewBeforePlay)
			{
				_settings._preview = _previewBeforePlay;
				_settingsDirty = true;
			}
		}
		ui::Node*   root = canvas.GetRootNode();
		ui::Node*   prefabRoot = tab->GetDisplayedPrefabRoot();

		// Laid out for the previewed device as it would be in game. Only the tab's runtime canvas state: never saved in the prefab.
		// Eased towards the device's resolution rather than snapping to it, so switching device (or rotating it) morphs the layout progressively.
		Vector2 previewGoal = _settings.GetPreviewResolution();
		if (previewGoal == Vector2::Zero)
		{
			previewGoal = canvas.GetEffectiveScaler()._designResolution;
		}
		if (_previewResolution == Vector2::Zero)
		{
			_previewResolution = previewGoal;
		}
		else
		{
			_previewResolution += (previewGoal - _previewResolution) * Min(1.0f, ImGui::GetIO().DeltaTime * 10.0f);
			Vector2 remaining = previewGoal - _previewResolution;
			if (std::abs(remaining.GetX()) < 0.5f && std::abs(remaining.GetY()) < 0.5f)
			{
				_previewResolution = previewGoal;
			}
		}
		canvas.SetTargetResolution(_previewResolution);
		canvas.UpdateLayout();
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

		bool viewportResized = false;
		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			_renderTarget->Init(resolutionWidth, resolutionHeight); // todo error
			viewportResized = true;
		}

		if (_renderTarget->IsValid() == false)
		{
			return;
		}

		// Uniform scale (pixels per canvas unit) that frames the device frame in the viewport at zoom = 1 (see Framing).
		// Fitted on the frame the preview is heading to, not the one currently drawn: the intermediate resolutions
		// pass through other aspect ratios (e.g. squarer while rotating), whose fit would zoom in then out again.
		// Eased on its own instead, so the camera goes straight to the final framing while the layout morphs.
		Vector2 goalLayoutSize;
		float   goalScaleFactor = 1.0f;
		canvas.GetEffectiveScaler().Compute(previewGoal, goalLayoutSize, goalScaleFactor);
		Vector2 goalFrameSize = previewGoal / goalScaleFactor;
		float   goalBaseScale = Framing::ComputeScale(goalFrameSize, Vector2((float)resolutionWidth, (float)resolutionHeight));
		if (_baseScale <= 0.0f || viewportResized)
		{
			_baseScale = goalBaseScale;
		}
		else
		{
			_baseScale += (goalBaseScale - _baseScale) * Min(1.0f, ImGui::GetIO().DeltaTime * 10.0f);
		}
		float baseScale = _baseScale;

		bool hovered = ImGui::IsWindowHovered() && mouseOverToolbar == false;
		// Playing, the wheel is the UI's (its scroll views): Ctrl + wheel zooms then
		if (hovered && ImGui::GetIO().MouseWheel != 0.0f && (playing == false || ImGui::GetIO().KeyCtrl))
		{
			_camera.ZoomBy(ImGui::GetIO().MouseWheel);
		}

		// F (here or in the hierarchy) frames the selected node. With the canvas selected (or nothing), the previewed device's
		// frame is framed instead, as when picking another device in the list.
		bool frameSelection = tab->ConsumeFrameSelectionRequest();
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_F, false) && ImGui::GetIO().WantTextInput == false)
		{
			frameSelection = true;
		}
		// Delete (here or in the hierarchy) removes the selected nodes. Before the selection is read below, nothing of it is drawn yet.
		if (playing == false && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_Delete, false) &&
		    ImGui::GetIO().WantTextInput == false && tab->GetSelectedNodes().Empty() == false && _draggedGizmoHandle == GizmoHandle::None &&
		    _marqueeState == MarqueeState::None)
		{
			tab->DeleteNodes(tab->GetSelectedNodes());
		}
		// Ctrl+C / Ctrl+X / Ctrl+V / Ctrl+Shift+V / Ctrl+D / Escape, as in the hierarchy. Same moment, for the same reason.
		if (playing == false && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().WantTextInput == false &&
		    _draggedGizmoHandle == GizmoHandle::None && _marqueeState == MarqueeState::None)
		{
			tab->HandleClipboardShortcuts();
		}

		bool frameDevice = _frameDeviceRequested;
		_frameDeviceRequested = false;

		Vector2    viewSize((float)resolutionWidth, (float)resolutionHeight);
		// The selection belongs to the edited canvas, not to the play mode's copy: no selection while playing.
		ui::Node* selectedNode = playing ? nullptr : tab->GetSelectedNode();
		if (frameSelection && frameDevice == false && selectedNode != nullptr)
		{
			// All the selected nodes
			Vector2 boundsCenter;
			Vector2 boundsSize;
			ComputeCanvasBounds(tab->GetSelectedNodes(), boundsCenter, boundsSize);
			_camera.Frame(boundsCenter, boundsSize, viewSize, baseScale, true);
		}
		else if (frameSelection || frameDevice)
		{
			// The device frame is centered on the canvas origin and framed at zoom 1 (see baseScale).
			_camera.AnimateTo(Vector2::Zero, 1.0f);
		}
		else if (viewportResized && _camera.HasBeenMoved() == false)
		{
			// The tab opens on the prefab root framed, already in place rather than animated there. Snapped again on each
			// viewport resize until the camera is moved: the docked windows of a freshly opened tab only get their final
			// size a few frames later, so the first frame's framing is computed for a viewport of another size.
			_camera.Frame(prefabRoot->ComputeCanvasMatrix().GetTranslation(), prefabRoot->GetSize(), viewSize, baseScale, false);
		}

		_camera.Update(ImGui::GetIO().DeltaTime);

		float scale = baseScale * _camera.GetZoom();

		if (hovered && ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
		{
			// Screen space is Y-down; canvas space is Y-up, hence the flip on Y.
			_camera.Pan(Vector2(-ImGui::GetIO().MouseDelta.x / scale, ImGui::GetIO().MouseDelta.y / scale));
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
		Matrix4 view = Matrix4::Translation(_camera.GetPosition());

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

		ui::Node* hitNode = nullptr;
		if (playing)
		{
			if (tab->ConsumeFrame())
			{
				hitNode = UpdateInteraction(canvas, mouseCanvasPos, hovered && mouseInsideImage);

				// The state transitions the pointer just started (or already playing), laid out again before drawing. A step
				// (paused) lasts a 60 Hz frame.
				canvas.Update(tab->IsPaused() ? 1.0f / 60.0f : ImGui::GetIO().DeltaTime);
				canvas.UpdateLayout();
			}
			else if (hovered && mouseInsideImage)
			{
				// Paused: the pointer is left as it was, the overlay still shows what is under the mouse
				hitNode = canvas.GetRootNode()->HitTest(mouseCanvasPos);
			}
		}
		else
		{
			_interactionPointerDown = false;
		}

		// Preview: nothing drawn over the prefab but the device frame (where the screen's edges are), so no gizmo to grab either
		bool               preview = _settings._preview;
		Vector<ui::Node*> gizmoNodes;
		if (selectedNode != nullptr && preview == false)
		{
			gizmoNodes = GetGizmoNodes();
		}
		if (gizmoNodes.Empty() == false)
		{
			UpdateGizmoDrag(gizmoNodes, mouseCanvasPos, scale, hovered && mouseInsideImage && _marqueeState == MarqueeState::None);
		}
		else
		{
			_hoveredGizmoHandle = GizmoHandle::None;
			_draggedGizmoHandle = GizmoHandle::None;
			_gizmoMovePending = false;
			_gizmoDragStarts.Clear();
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
		MaterialInstance* backgroundMaterial = MaterialInstance::Create(
			BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		backgroundMaterial->SetVec4("ubo.color", Vector4(30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f));

		StaticArray<Vector2, 4> backgroundVertices = {
			Vector2(-canvasSize.GetX() * 0.5f, canvasSize.GetY() * 0.5f),
			Vector2(canvasSize.GetX() * 0.5f, canvasSize.GetY() * 0.5f),
			Vector2(canvasSize.GetX() * 0.5f, -canvasSize.GetY() * 0.5f),
			Vector2(-canvasSize.GetX() * 0.5f, -canvasSize.GetY() * 0.5f),
		};
		StaticArray<uint16_t, 6> backgroundIndices = {0, 1, 2, 0, 2, 3};

		RenderCommandMesh* backgroundCommand = DefaultAllocator::GetInstance().New<RenderCommandMesh>(
			backgroundVertices.Data(), nullptr, nullptr, (uint32_t)backgroundVertices.Size(), backgroundIndices.Data(), (uint32_t)backgroundIndices.Size(),
			Matrix4::Identity, backgroundMaterial, 0);
		renderView->PushRenderCommand(backgroundCommand);
		renderView->DeleteAfter(backgroundMaterial);
*/
		// The grid is aligned on the canvas' top-left corner (the canvas rect is centered on the canvas origin).
		Vector2 gridOrigin(-canvasSize.GetX() * 0.5f, canvasSize.GetY() * 0.5f);
		if (_settings._gridVisible && preview == false)
		{
			ViewportGrid grid;
			grid._origin = gridOrigin;
			grid._cellSize = _settings._gridCellSize;
			grid._viewCenter = _camera.GetPosition();
			grid._viewHalfSize = Vector2(worldHalfWidth, worldHalfHeight);
			grid._pixelsPerUnit = scale;
			grid._zoom = _camera.GetZoom();
			grid.Draw(*renderView);
		}
		DrawPreviewFrames(*renderView);

		// The actual content first: the outlines/gizmos below are pushed with a later (or the maximum)
		// rendering order, so they stay on top of it.
		canvas.PushRenderCommand(*renderView, RenderView::RenderQueueType::World);

		if (preview == false)
		{
			// Only the prefab root's subtree: `root` is the hidden container, whose rect is the design resolution, not part of the prefab.
			DrawNode(prefabRoot, *renderView);
			if (playing)
			{
				// Over the outlines: what the pointer reaches
				DrawInteractionOverlay(canvas, hitNode, *renderView);
			}

			if (selectedNode != nullptr)
			{
				DrawSpacing(selectedNode, *renderView);
			}

			if (gizmoNodes.Empty() == false)
			{
				DrawSelectionGizmo(gizmoNodes, scale, *renderView);
			}
		}

		ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));
		if (tab->IsRecording() || tab->IsRecordingAnimation())
		{
			// Framed in red as long as the edits record a state (or animation keys) rather than change the prefab
			ImGui::GetWindowDrawList()->AddRect(imagePos, imagePos + ImVec2((float)resolutionWidth, (float)resolutionHeight), IM_COL32(230, 60, 60, 255), 0.0f, 3.0f);
		}
		else if (playing)
		{
			// Framed in blue as long as the play mode's copy is shown rather than the prefab (orange once paused)
			ImU32 color = tab->IsPaused() ? IM_COL32(230, 160, 40, 255) : IM_COL32(60, 140, 230, 255);
			ImGui::GetWindowDrawList()->AddRect(imagePos, imagePos + ImVec2((float)resolutionWidth, (float)resolutionHeight), color, 0.0f, 3.0f);
		}
		DrawPreviewLabels(imagePos, ImVec2((float)resolutionWidth, (float)resolutionHeight), scale);

		// The play mode's copy has no selection, and nodes rebuilt since the press (undo...) leave stale pointers: the rectangle is dropped
		if (_marqueeState != MarqueeState::None && (playing || tab->GetRebuildCount() != _marqueeRebuildCount))
		{
			_marqueeState = MarqueeState::None;
			_marqueeSelectionBefore.Clear();
		}

		if (playing == false && hovered && mouseInsideImage && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && _draggedGizmoHandle == GizmoHandle::None)
		{
			// Before the click changes the selection: a drag turns it into a selection rectangle
			StartMarquee(mouseCanvasPos);

			// Ctrl or Shift adds the picked node to the selection, or removes it from it
			ui::Node* pickedNode = PickNode(prefabRoot, mouseCanvasPos);
			if (ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyShift)
			{
				tab->SetNodeSelected(pickedNode, tab->IsSelected(pickedNode) == false);
			}
			else
			{
				tab->SetSelectedNode(pickedNode);
			}
		}

		if (_marqueeState != MarqueeState::None)
		{
			UpdateMarquee(prefabRoot, mouseCanvasPos);
			DrawMarquee(imagePos, ImVec2((float)resolutionWidth, (float)resolutionHeight), scale);
		}

		if (playing == false && ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UINodeLibraryDescriptor");
			if (payload != nullptr)
			{
				ReflectionDescriptor* nodeDescriptor = *static_cast<ReflectionDescriptor**>(payload->Data);

				// In the selection, or the closest node that can have new children (not an instance's)
				ui::Node* parent = tab->GetSelectedNode();
				while (parent != nullptr && UIPrefabEditorTab::CanEditStructure(parent) == false)
				{
					parent = parent->GetParent();
				}
				if (parent == nullptr)
				{
					parent = tab->GetPrefabRoot();
				}

				Vector2 dropCanvasPos = screenToCanvas(ImGui::GetIO().MousePos);
				Vector2 parentCanvasPos = parent->ComputeCanvasMatrix().GetTranslation();
				// Parent rotation/scale ignored when converting to a local offset, same simplification as DrawNode/PickNode.
				Vector2 localOffset = dropCanvasPos - parentCanvasPos;

				ui::Node*    newNode = ui::NodeFactory::GetInstance()->CreateNode(*nodeDescriptor);
				LayoutParams* layoutParams = parent->CreateDefaultLayoutParams();
				if (AnchoredLayoutParams* anchoredLayoutParams = LayoutParams::Cast<AnchoredLayoutParams>(layoutParams))
				{
					anchoredLayoutParams->SetOffset(localOffset);
				}
				parent->AddChild(newNode, layoutParams);

				tab->SetSelectedNode(newNode);
				tab->MarkAsDirty(fmt::format("Add {}", nodeDescriptor->GetDisplayName()));
			}
			ImGui::EndDragDropTarget();
		}
	}

	/// @brief Toolbar on top of the viewport: the previewed device (a drop down listing the devices, plus a
	/// rotate toggle while one is active) and a "Grid" toggle glued to a small arrow button opening its settings
	/// (see DrawHelper::DrawToggleWithSettings).
	void UIPrefabEditorViewportWindow::DrawToolbar()
	{
		// The window itself has no padding (the viewport image goes edge to edge), keep the toolbar off the borders
		// and off the hidden tab bar triangle.
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImGui::GetStyle().WindowPadding + ImVec2(DrawHelper::GetHiddenTabBarButtonWidth(), 0.0f));

		String deviceLabel = ICON_MDI_MONITOR_CELLPHONE " ";
		if (_settings._activeDevice >= 0)
		{
			deviceLabel += _settings._devices[(uint32_t)_settings._activeDevice]._name;
		}
		else
		{
			deviceLabel += "Design resolution";
		}
		DrawHelper::DrawDropDownButton(deviceLabel.CStr(), "Preview device", "UIPrefabDevices");

		if (_settings._activeDevice >= 0)
		{
			ImGui::SameLine();
			if (FramedButton(ICON_MDI_SCREEN_ROTATION, _settings._deviceRotated))
			{
				_settings._deviceRotated = !_settings._deviceRotated;
				_settingsDirty = true;
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			{
				ImGui::SetTooltip(_settings._deviceRotated ? "Rotated (width and height swapped)" : "Rotate (swap width and height)");
			}
		}

		ImGui::SameLine();
		_settingsDirty |= DrawHelper::DrawToggleWithSettings(ICON_MDI_GRID " Grid", "grid", "UIPrefabGrid", _settings._gridVisible);

		ImGui::SameLine();
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		if (tab->IsRecording())
		{
			String label = String(ICON_MDI_STOP " Recording ") + tab->GetRecordedStateName();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
			if (FramedButton(label.CStr(), true))
			{
				tab->StopRecording();
			}
			ImGui::PopStyleColor();
			ImGui::SetItemTooltip("Edits to colors, scales... record the state's overrides. Click to stop and go back to the prefab as authored");
			ImGui::SameLine();
		}
		else if (tab->IsRecordingAnimation() && tab->GetPreviewedAnimation() != nullptr)
		{
			String label = String(ICON_MDI_STOP " Recording ") + tab->GetPreviewedAnimation()->_name;
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
			if (FramedButton(label.CStr(), true))
			{
				tab->SetRecordingAnimation(false);
			}
			ImGui::PopStyleColor();
			ImGui::SetItemTooltip("Edits to colors, scales... add keys to the animation at the timeline's time. Click to stop recording");
			ImGui::SameLine();
		}
		if (FramedButton(ICON_MDI_EYE " Preview", _settings._preview))
		{
			_settings._preview = !_settings._preview;
			_settingsDirty = true;
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip(_settings._preview ? "Back to the grid, outlines and gizmos"
			                                     : "Only the prefab as drawn in game, in its device frame: no grid, outlines nor gizmos");
		}

		DrawDevicesPopup();
		DrawGridSettingsPopup();

		// Saved once the user is done editing (not on every keystroke/drag step): as soon as no widget is active anymore.
		if (_settingsDirty && ImGui::IsAnyItemActive() == false)
		{
			_settings.Save();
			_settingsDirty = false;
		}
	}

	/// @brief Device list, one row each: a radio button marking the previewed one (click to preview it), then its
	/// name, resolution and delete button. The first row is the canvas' design resolution, read-only (edited
	/// in the canvas scaler). Followed by an "Add Device" and a "Reset" button.
	void UIPrefabEditorViewportWindow::DrawDevicesPopup()
	{
		if (DrawHelper::BeginSettingsPopup("UIPrefabDevices") == false)
		{
			return;
		}

		UIPrefabEditorTab*       tab = GetOwner<UIPrefabEditorTab>();
		const ui::CanvasScaler& scaler = tab->GetCanvas().GetEffectiveScaler();

		constexpr float nameWidth = 140.0f;
		constexpr float resolutionWidth = 170.0f;

		ImGui::PushID("DesignResolution");
		if (ImGui::RadioButton("##Active", _settings._activeDevice < 0))
		{
			_settings._activeDevice = -1;
			_settingsDirty = true;
			_frameDeviceRequested = true;
		}
		ImGui::SameLine();
		ImGui::BeginDisabled();
		char designName[] = "Design resolution";
		ImGui::SetNextItemWidth(nameWidth);
		ImGui::InputText("##Name", designName, sizeof(designName), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		int32_t designResolution[2] = {(int32_t)scaler._designResolution.GetX(), (int32_t)scaler._designResolution.GetY()};
		ImGui::SetNextItemWidth(resolutionWidth);
		ImGui::InputInt2("##Resolution", designResolution, ImGuiInputTextFlags_ReadOnly);
		ImGui::EndDisabled();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("The project's (Edit > Project Settings), or the Canvas Scaler's when the canvas overrides it (select the Canvas in the hierarchy)");
		}
		ImGui::PopID();

		int32_t removeIndex = -1;
		for (uint32_t i = 0; i < _settings._devices.Size(); ++i)
		{
			Device& device = _settings._devices[i];

			ImGui::PushID((int)i);

			if (ImGui::RadioButton("##Active", _settings._activeDevice == (int32_t)i))
			{
				_settings._activeDevice = (int32_t)i;
				_settingsDirty = true;
				_frameDeviceRequested = true;
			}
			ImGui::SameLine();

			char nameBuffer[128] = {'\0'};
			std::strncpy(nameBuffer, device._name.CStr(), sizeof(nameBuffer) - 1);
			ImGui::SetNextItemWidth(nameWidth);
			if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
			{
				device._name = nameBuffer;
				_settingsDirty = true;
			}

			ImGui::SameLine();
			int32_t resolution[2] = {(int32_t)device._resolution.GetX(), (int32_t)device._resolution.GetY()};
			ImGui::SetNextItemWidth(resolutionWidth);
			if (ImGui::InputInt2("##Resolution", resolution))
			{
				device._resolution = Vector2((float)Max(resolution[0], 1), (float)Max(resolution[1], 1));
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
			_settings._devices.Erase((uint32_t)removeIndex);
			if (_settings._activeDevice == removeIndex)
			{
				_settings._activeDevice = -1;
			}
			else if (_settings._activeDevice > removeIndex)
			{
				--_settings._activeDevice;
			}
			_settingsDirty = true;
		}

		if (_settings._devices.Empty())
		{
			ImGui::TextDisabled("No device");
		}

		if (ImGui::Button(ICON_MDI_PLUS " Add Device"))
		{
			Device& device = _settings._devices.EmplaceBack();
			device._name = String("Device ") + String(std::to_string(_settings._devices.Size()).c_str());
			device._resolution = scaler._designResolution;
			_settingsDirty = true;
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_RESTORE " Reset"))
		{
			_settings.ResetDevices();
			_settingsDirty = true;
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Replace the devices by the built-in list");
		}

		ImGui::EndPopup();
	}

	/// @brief Grid settings: size of a cell at zoom 1.0 (the grid still coarsens/refines by
	/// powers of two as the zoom changes, see ViewportGrid).
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

	/// @brief Size, in canvas units, of the previewed screen (at its current, possibly still easing, resolution): the canvas'
	/// layout size, except in CanvasScaler::Mode::Fit where the screen can be larger than the (design sized) canvas, the
	/// difference being the letterbox bars.
	Vector2 UIPrefabEditorViewportWindow::GetDeviceFrameSize() const
	{
		const ui::Canvas& canvas = const_cast<UIPrefabEditorViewportWindow*>(this)->GetOwner<UIPrefabEditorTab>()->GetCanvas();
		if (canvas.GetScaleFactor() <= 0.0f)
		{
			return canvas.GetLayoutSize();
		}
		return canvas.GetTargetResolution() / canvas.GetScaleFactor();
	}

	/// @brief Outlines the previewed device's frame, centered on the canvas origin like the canvas itself. Its label is
	/// drawn afterwards over the blitted image, see DrawPreviewLabels.
	void UIPrefabEditorViewportWindow::DrawPreviewFrames(RenderView& renderView)
	{
		ShapeDrawer::Rect(renderView, Matrix4::Identity, Vector2::Zero, GetDeviceFrameSize(), Color(0.85f, 0.85f, 0.85f, 1.0f), GizmoViewport::OverlayOrder);
	}

	/// @brief Labels the frame of DrawPreviewFrames with the device's name and resolution, above its top-left corner.
	/// Done with ImGui over the image (the render target can't draw text), hence the manual canvas -> screen projection,
	/// the inverse of DrawViewport's screenToCanvas.
	/// @param imagePos screen position of the image's top-left corner
	/// @param imageSize
	/// @param scale pixels per canvas unit
	void UIPrefabEditorViewportWindow::DrawPreviewLabels(const ImVec2& imagePos, const ImVec2& imageSize, float scale)
	{
		const ui::CanvasScaler& scaler = GetOwner<UIPrefabEditorTab>()->GetCanvas().GetEffectiveScaler();

		// Canvas space is Y-up, screen space Y-down; the canvas point at the view's center is _camera.GetPosition().
		auto canvasToScreen = [&](float x, float y) -> ImVec2
		{ return ImVec2(imagePos.x + imageSize.x * 0.5f + (x - _camera.GetPosition().GetX()) * scale, imagePos.y + imageSize.y * 0.5f - (y - _camera.GetPosition().GetY()) * scale); };

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(imagePos, imagePos + imageSize, true);

		Vector2 frameSize = GetDeviceFrameSize();
		Vector2 previewResolution = _settings.GetPreviewResolution();
		if (previewResolution == Vector2::Zero)
		{
			previewResolution = scaler._designResolution;
		}

		char frameLabel[192];
		if (_settings._activeDevice >= 0)
		{
			std::snprintf(frameLabel, sizeof(frameLabel), "%s  %d x %d", _settings._devices[(uint32_t)_settings._activeDevice]._name.CStr(), (int)previewResolution.GetX(),
			              (int)previewResolution.GetY());
		}
		else
		{
			std::snprintf(frameLabel, sizeof(frameLabel), "Design resolution  %d x %d", (int)previewResolution.GetX(), (int)previewResolution.GetY());
		}
		ImVec2 frameTopLeft = canvasToScreen(-frameSize.GetX() * 0.5f, frameSize.GetY() * 0.5f);
		ImVec2 frameLabelSize = ImGui::CalcTextSize(frameLabel);
		drawList->AddText(ImVec2(frameTopLeft.x, frameTopLeft.y - frameLabelSize.y - 2.0f), IM_COL32(217, 217, 217, 255), frameLabel);

		drawList->PopClipRect();
	}

	/// @brief
	/// @param node
	/// @param renderView
	/// @brief Feeds the play mode's canvas the mouse, as the game's input would be. The button only counts as down for a press
	/// started over the viewport (not one started elsewhere in the editor then dragged in), and the pointer stays tracked outside of
	/// the viewport until it is released, as a game window captures the mouse.
	/// @param canvas the play mode's canvas, laid out
	/// @param canvasMousePosition
	/// @param mouseAvailable whether the mouse is over the viewport's image, and not used by the toolbar (its wheel then scrolls
	/// the UI, unless Ctrl zooms the view)
	/// @return the node the hit-test finds under the mouse, null if none or not over the viewport
	ui::Node* UIPrefabEditorViewportWindow::UpdateInteraction(ui::Canvas& canvas, const Vector2& canvasMousePosition, bool mouseAvailable)
	{
		if (mouseAvailable && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			_interactionPointerDown = true;
		}
		else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
		{
			_interactionPointerDown = false;
		}

		if (mouseAvailable == false && _interactionPointerDown == false)
		{
			canvas.ClearPointer();
			return nullptr;
		}

		// ImGui's horizontal wheel is > 0 towards the left
		const ImGuiIO& io = ImGui::GetIO();
		Vector2        scroll = (mouseAvailable && io.KeyCtrl == false) ? Vector2(-io.MouseWheelH, io.MouseWheel) : Vector2::Zero;
		canvas.UpdatePointer(canvasMousePosition, _interactionPointerDown, scroll);
		return mouseAvailable ? canvas.GetRootNode()->HitTest(canvasMousePosition) : nullptr;
	}

	/// @brief Play mode's overlay: what the pointer reaches. The node the hit-test finds under the mouse in white, and the
	/// node whose Interaction gets the pointer (hovered, or holding the press) colored after its state when it is a Selectable.
	/// Drawn with their canvas matrix: exactly the rect the hit-test uses, rotated and scaled, unlike the edit mode's outlines.
	/// @param canvas the play mode's canvas
	/// @param hitNode
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawInteractionOverlay(ui::Canvas& canvas, ui::Node* hitNode, RenderView& renderView)
	{
		if (hitNode != nullptr)
		{
			ShapeDrawer::Rect(renderView, hitNode->ComputeCanvasMatrix(), Vector2::Zero, hitNode->GetSize(), Color(1.0f, 1.0f, 1.0f, 0.6f), GizmoViewport::OverlayOrder);
		}

		auto drawInteractive = [&](ui::Node* node)
		{
			Color color(0.0f, 170.0f / 255.0f, 1.0f, 1.0f);
			if (const ui::Selectable* selectable = node->GetInteraction<ui::Selectable>())
			{
				switch (selectable->GetState())
				{
					case ui::Selectable::State::Normal: break;
					case ui::Selectable::State::Hovered: color = Color(1.0f, 200.0f / 255.0f, 50.0f / 255.0f, 1.0f); break;
					case ui::Selectable::State::Pressed: color = Color(80.0f / 255.0f, 220.0f / 255.0f, 100.0f / 255.0f, 1.0f); break;
					case ui::Selectable::State::Disabled: color = Color(0.5f, 0.5f, 0.5f, 1.0f); break;
				}
			}
			ShapeDrawer::Rect(renderView, node->ComputeCanvasMatrix(), Vector2::Zero, node->GetSize(), color, GizmoViewport::OverlayOrder);
		};

		if (canvas.GetHoveredNode() != nullptr)
		{
			drawInteractive(canvas.GetHoveredNode());
		}
		if (canvas.GetPressedNode() != nullptr && canvas.GetPressedNode() != canvas.GetHoveredNode())
		{
			drawInteractive(canvas.GetPressedNode());
		}
	}

	void UIPrefabEditorViewportWindow::DrawNode(ui::Node* node, RenderView& renderView)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		Vector2 size = node->GetSize();
		Vector2 canvasPosition = node->ComputeCanvasMatrix().GetTranslation();

		bool  isSelected = tab->IsSelected(node);
		Color color = isSelected ? Color(1.0f, 165.0f / 255.0f, 0.0f, 1.0f) : Color(120.0f / 255.0f, 170.0f / 255.0f, 1.0f, 180.0f / 255.0f);

		ShapeDrawer::Rect(renderView, Matrix4::Translation(canvasPosition), Vector2::Zero, size, color, GizmoViewport::OverlayOrder);

		for (ui::Node* child : node->GetChildren())
		{
			DrawNode(child, renderView);
		}
	}

	/// @brief The selected node's spacing, filled as in a browser's inspector: its padding (inside its rect, around its children)
	/// in green when it has a BoxLayout, its margin (outside its rect, pushing its neighbours away) in orange when its parent has one.
	/// Axis-aligned, as the outlines (see DrawNode).
	/// @param node
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawSpacing(ui::Node* node, RenderView& renderView)
	{
		constexpr uint32_t LayoutFrameOrder = GizmoViewport::OverlayOrder - 1; // under the outlines

		Rect rect;
		rect._size = node->GetSize();
		rect._position = node->ComputeCanvasMatrix().GetTranslation() - rect._size * 0.5f;

		if (const ui::BoxLayout* boxLayout = node->GetLayout<ui::BoxLayout>())
		{
			Rect inner = rect.Inset(boxLayout->GetPadding());
			ShapeDrawer::FilledFrame(renderView, Matrix4::Identity, rect._position, rect._size, inner._position, inner._size,
			                         Color(147.0f / 255.0f, 196.0f / 255.0f, 125.0f / 255.0f, 0.55f), LayoutFrameOrder);
		}

		if (const ui::BoxLayoutParams* boxLayoutParams = LayoutParams::Cast<ui::BoxLayoutParams>(node->GetLayoutParams()))
		{
			const ui::Margin& margin = boxLayoutParams->GetMargin();
			Rect               outer;
			outer._position = rect._position - Vector2(margin._left, margin._bottom);
			outer._size = rect._size + Vector2(margin._left + margin._right, margin._bottom + margin._top);
			ShapeDrawer::FilledFrame(renderView, Matrix4::Identity, outer._position, outer._size, rect._position, rect._size,
			                         Color(246.0f / 255.0f, 178.0f / 255.0f, 107.0f / 255.0f, 0.55f), LayoutFrameOrder);
		}
	}

	/// @brief
	/// @param node
	/// @param canvasPosition
	/// @return
	ui::Node* UIPrefabEditorViewportWindow::PickNode(ui::Node* node, const Vector2& canvasPosition)
	{
		ui::Node* picked = nullptr;

		Vector2 size = node->GetSize();
		Vector2 nodeCanvasPosition = node->ComputeCanvasMatrix().GetTranslation();
		Vector2 halfExtent = size * 0.5f;

		if (canvasPosition.GetX() >= nodeCanvasPosition.GetX() - halfExtent.GetX() && canvasPosition.GetX() <= nodeCanvasPosition.GetX() + halfExtent.GetX() &&
		    canvasPosition.GetY() >= nodeCanvasPosition.GetY() - halfExtent.GetY() && canvasPosition.GetY() <= nodeCanvasPosition.GetY() + halfExtent.GetY())
		{
			picked = node;
		}

		for (ui::Node* child : node->GetChildren())
		{
			ui::Node* childPicked = PickNode(child, canvasPosition);
			if (childPicked != nullptr)
			{
				picked = childPicked;
			}
		}

		return picked;
	}

	/// @brief A left press on the image, not taken by the gizmo: still a click (see DrawViewport) until dragged past ImGui's drag threshold
	/// @param canvasMousePosition
	void UIPrefabEditorViewportWindow::StartMarquee(const Vector2& canvasMousePosition)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		_marqueeState = MarqueeState::Pending;
		_marqueeStart = canvasMousePosition;
		_marqueeAdditive = ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyShift;
		_marqueeSelectionBefore = tab->GetSelectedNodes();
		_marqueeRebuildCount = tab->GetRebuildCount();
	}

	/// @brief Follows the press started by StartMarquee: once dragged, the selection is the outermost nodes entirely inside the rectangle
	/// (a node inside one of them comes with it, see UIPrefabEditorTab::GetSelectedSubtrees), in the hierarchy's order, after the selection
	/// from before the press when additive. Recomputed each frame from the press' selection, so shrinking the rectangle deselects again.
	/// Ends on release, Escape gives back the selection from before the press.
	/// @param prefabRoot
	/// @param canvasMousePosition
	void UIPrefabEditorViewportWindow::UpdateMarquee(ui::Node* prefabRoot, const Vector2& canvasMousePosition)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
		{
			_marqueeState = MarqueeState::None;
			_marqueeSelectionBefore.Clear();
			return;
		}

		if (_marqueeState == MarqueeState::Cancelled)
		{
			return;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
		{
			if (_marqueeState == MarqueeState::Active)
			{
				tab->SetSelectedNodes(_marqueeSelectionBefore);
			}
			_marqueeState = MarqueeState::Cancelled;
			return;
		}

		if (_marqueeState == MarqueeState::Pending)
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) == false)
			{
				return;
			}
			_marqueeState = MarqueeState::Active;
		}

		Vector2 min(Min(_marqueeStart.GetX(), canvasMousePosition.GetX()), Min(_marqueeStart.GetY(), canvasMousePosition.GetY()));
		Vector2 max(Max(_marqueeStart.GetX(), canvasMousePosition.GetX()), Max(_marqueeStart.GetY(), canvasMousePosition.GetY()));

		Vector<ui::Node*> inside;
		CollectNodesInRect(prefabRoot, min, max, inside);

		Vector<ui::Node*> selection;
		if (_marqueeAdditive)
		{
			selection = _marqueeSelectionBefore;
		}
		for (ui::Node* node : inside)
		{
			if (std::find(selection.Begin(), selection.End(), node) == selection.End())
			{
				selection.PushBack(node);
			}
		}
		tab->SetSelectedNodes(selection);
	}

	/// @brief The rectangle, over the image (ImGui, in screen space: crisp at any zoom), from its canvas-space start to the mouse
	/// @param imagePos screen position of the image's top-left corner
	/// @param imageSize
	/// @param scale pixels per canvas unit
	void UIPrefabEditorViewportWindow::DrawMarquee(const ImVec2& imagePos, const ImVec2& imageSize, float scale)
	{
		if (_marqueeState != MarqueeState::Active)
		{
			return;
		}

		// Canvas space is Y-up, screen space Y-down (see DrawPreviewLabels)
		ImVec2 start(imagePos.x + imageSize.x * 0.5f + (_marqueeStart.GetX() - _camera.GetPosition().GetX()) * scale,
		             imagePos.y + imageSize.y * 0.5f - (_marqueeStart.GetY() - _camera.GetPosition().GetY()) * scale);
		ImVec2 end = ImGui::GetIO().MousePos;
		ImVec2 min(Min(start.x, end.x), Min(start.y, end.y));
		ImVec2 max(Max(start.x, end.x), Max(start.y, end.y));

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(imagePos, imagePos + imageSize, true);
		drawList->AddRectFilled(min, max, IM_COL32(120, 170, 255, 40));
		drawList->AddRect(min, max, IM_COL32(120, 170, 255, 220));
		drawList->PopClipRect();
	}

	/// @brief The outermost nodes of node's subtree whose rect (axis-aligned, as the outlines, see DrawNode) is entirely inside [min, max],
	/// depth first: the hierarchy's order
	/// @param node
	/// @param min
	/// @param max
	/// @param nodes
	void UIPrefabEditorViewportWindow::CollectNodesInRect(ui::Node* node, const Vector2& min, const Vector2& max, Vector<ui::Node*>& nodes)
	{
		Vector2 center = node->ComputeCanvasMatrix().GetTranslation();
		Vector2 halfSize = node->GetSize() * 0.5f;
		if (center.GetX() - halfSize.GetX() >= min.GetX() && center.GetX() + halfSize.GetX() <= max.GetX() && center.GetY() - halfSize.GetY() >= min.GetY() &&
		    center.GetY() + halfSize.GetY() <= max.GetY())
		{
			nodes.PushBack(node);
			return;
		}

		for (ui::Node* child : node->GetChildren())
		{
			CollectNodesInRect(child, min, max, nodes);
		}
	}

	/// @brief What the gizmo may do to the node, from what places it. AnchoredLayoutParams: everything. A BoxLayout's child: never moved
	/// (the layout places it), resized on its main axis when it has its own size there (not Fill), on its cross axis when it is not stretched
	/// @param node
	/// @return
	UIPrefabEditorViewportWindow::GizmoAxes UIPrefabEditorViewportWindow::GetGizmoAxes(const ui::Node& node)
	{
		GizmoAxes axes;
		// Inside an instance held by an instance, nothing can be edited
		if (UIPrefabEditorTab::CanEditProperties(&node) == false)
		{
			return axes;
		}

		const LayoutParams* layoutParams = node.GetLayoutParams();
		if (LayoutParams::Cast<AnchoredLayoutParams>(layoutParams) != nullptr)
		{
			axes._move = true;
			axes._resizeX = true;
			axes._resizeY = true;
		}
		else if (const ui::HorizontalLayoutParams* horizontalLayoutParams = LayoutParams::Cast<ui::HorizontalLayoutParams>(layoutParams))
		{
			axes._resizeX = horizontalLayoutParams->GetRule() == ui::BoxLayoutParams::Rule::OwnSize;
			axes._resizeY = horizontalLayoutParams->GetVerticalAlign() != ui::HorizontalLayoutParams::VerticalAlign::Stretch;
		}
		else if (const ui::VerticalLayoutParams* verticalLayoutParams = LayoutParams::Cast<ui::VerticalLayoutParams>(layoutParams))
		{
			axes._resizeX = verticalLayoutParams->GetHorizontalAlign() != ui::VerticalLayoutParams::HorizontalAlign::Stretch;
			axes._resizeY = verticalLayoutParams->GetRule() == ui::BoxLayoutParams::Rule::OwnSize;
		}
		return axes;
	}

	namespace
	{
		constexpr float gizmoCornerHandlePixelSize = 8.0f;
		constexpr float gizmoEdgeHandlePixelThickness = 6.0f;

		/// @brief Filled quad in canvas space, the corner handles (an outline would be hard to see at handle size), over the outlines
		void DrawFilledQuad(const Vector2& center, const Vector2& size, const Color& color, RenderView& renderView)
		{
			ShapeDrawer::FilledRect(renderView, Matrix4::Identity, center, size, color, std::numeric_limits<uint32_t>::max());
		}
	}

	/// @brief With a single node selected: that node if the gizmo can do anything to it. With several: the roots of the selected
	/// subtrees that can move (a selected child moves with its selected parent, not twice), the ones a BoxLayout places left out.
	/// @return empty when there is nothing to manipulate
	Vector<ui::Node*> UIPrefabEditorViewportWindow::GetGizmoNodes() const
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();

		Vector<ui::Node*> nodes;
		if (tab->GetSelectedNodes().Size() == 1)
		{
			ui::Node* node = tab->GetSelectedNode();
			if (node->GetLayoutParams() != nullptr && GetGizmoAxes(*node).Any())
			{
				nodes.PushBack(node);
			}
			return nodes;
		}

		for (ui::Node* node : tab->GetSelectedSubtrees())
		{
			if (node->GetLayoutParams() != nullptr && GetGizmoAxes(*node)._move)
			{
				nodes.PushBack(node);
			}
		}
		return nodes;
	}

	/// @brief The canvas-space box around the nodes' rects (axis-aligned, as the outlines, see DrawNode)
	/// @param nodes not empty
	void UIPrefabEditorViewportWindow::ComputeCanvasBounds(const Vector<ui::Node*>& nodes, Vector2& center, Vector2& size)
	{
		Vector2 min;
		Vector2 max;
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			Vector2 nodeCenter = nodes[index]->ComputeCanvasMatrix().GetTranslation();
			Vector2 halfSize = nodes[index]->GetSize() * 0.5f;
			Vector2 nodeMin = nodeCenter - halfSize;
			Vector2 nodeMax = nodeCenter + halfSize;
			if (index == 0)
			{
				min = nodeMin;
				max = nodeMax;
			}
			else
			{
				min = Vector2(Min(min.GetX(), nodeMin.GetX()), Min(min.GetY(), nodeMin.GetY()));
				max = Vector2(Max(max.GetX(), nodeMax.GetX()), Max(max.GetY(), nodeMax.GetY()));
			}
		}
		center = (min + max) * 0.5f;
		size = max - min;
	}

	/// @brief Draws the move/resize handles over the selected node: a filled square at each of the
	/// 4 corners (resize keeping the opposite corner fixed) and a highlighted edge line when
	/// hovering/dragging one of the 4 edges (resize along one axis only). The node's body itself
	/// (already drawn as an outline by DrawNode) acts as the move handle.
	/// With several nodes, only the box around them (see ComputeCanvasBounds), highlighted while they are hovered or dragged: their bodies
	/// move them all, nothing resizes them.
	/// No rotation support: this viewport's overlays are axis-aligned, see the class comment on the header.
	/// @param nodes see GetGizmoNodes
	/// @param scale pixels per canvas unit, used to keep the handle size constant on screen
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawSelectionGizmo(const Vector<ui::Node*>& nodes, float scale, RenderView& renderView)
	{
		Color handleColor(0.25f, 0.25f, 1.0f, 1.0f);
		Color handleHighlightColor(0.5f, 0.5f, 1.0f, 1.0f);

		GizmoHandle activeHandle = _draggedGizmoHandle != GizmoHandle::None ? _draggedGizmoHandle : _hoveredGizmoHandle;

		auto isActive = [&](GizmoHandle handle) { return activeHandle == handle; };

		if (nodes.Size() > 1)
		{
			Vector2 boundsCenter;
			Vector2 boundsSize;
			ComputeCanvasBounds(nodes, boundsCenter, boundsSize);
			ShapeDrawer::Rect(renderView, Matrix4::Translation(boundsCenter), Vector2::Zero, boundsSize, isActive(GizmoHandle::Move) ? handleHighlightColor : handleColor, GizmoViewport::OverlayOrder);
			return;
		}

		ui::Node* node = nodes[0];
		Vector2    size = node->GetSize();
		Vector2    canvasPosition = node->ComputeCanvasMatrix().GetTranslation();
		Vector2    halfSize = size * 0.5f;

		float cornerSize = gizmoCornerHandlePixelSize / scale;

		Vector2 topLeft = canvasPosition + Vector2(-halfSize.GetX(), halfSize.GetY());
		Vector2 topRight = canvasPosition + Vector2(halfSize.GetX(), halfSize.GetY());
		Vector2 bottomLeft = canvasPosition + Vector2(-halfSize.GetX(), -halfSize.GetY());
		Vector2 bottomRight = canvasPosition + Vector2(halfSize.GetX(), -halfSize.GetY());

		DrawFilledQuad(topLeft, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::TopLeft) ? handleHighlightColor : handleColor, renderView);
		DrawFilledQuad(topRight, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::TopRight) ? handleHighlightColor : handleColor, renderView);
		DrawFilledQuad(bottomLeft, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::BottomLeft) ? handleHighlightColor : handleColor, renderView);
		DrawFilledQuad(bottomRight, Vector2(cornerSize, cornerSize), isActive(GizmoHandle::BottomRight) ? handleHighlightColor : handleColor, renderView);

		if (isActive(GizmoHandle::Top))
		{
			ShapeDrawer::Line(renderView, Matrix4::Identity, topLeft, topRight, handleHighlightColor, GizmoViewport::OverlayOrder);
		}
		if (isActive(GizmoHandle::Bottom))
		{
			ShapeDrawer::Line(renderView, Matrix4::Identity, bottomLeft, bottomRight, handleHighlightColor, GizmoViewport::OverlayOrder);
		}
		if (isActive(GizmoHandle::Left))
		{
			ShapeDrawer::Line(renderView, Matrix4::Identity, topLeft, bottomLeft, handleHighlightColor, GizmoViewport::OverlayOrder);
		}
		if (isActive(GizmoHandle::Right))
		{
			ShapeDrawer::Line(renderView, Matrix4::Identity, topRight, bottomRight, handleHighlightColor, GizmoViewport::OverlayOrder);
		}
	}

	/// @brief CPU hit-test of the gizmo handles against a canvas-space mouse position (this
	/// viewport has no RHI picking texture, unlike ViewportWindow/Handles::FreeMoveRect). Corners
	/// are tested first so their (small) area takes priority over the edge bands that reach them.
	/// With several nodes: Move over any of them (not the gaps of their box, where a click selects what is there).
	/// @param nodes see GetGizmoNodes
	/// @param canvasMousePosition
	/// @param scale pixels per canvas unit
	/// @return
	UIPrefabEditorViewportWindow::GizmoHandle UIPrefabEditorViewportWindow::HitTestGizmoHandle(const Vector<ui::Node*>& nodes, const Vector2& canvasMousePosition,
	                                                                                           float scale) const
	{
		auto within = [&](ui::Node& node)
		{
			Vector2 center = node.ComputeCanvasMatrix().GetTranslation();
			Vector2 halfSize = node.GetSize() * 0.5f;
			return std::abs(canvasMousePosition.GetX() - center.GetX()) <= halfSize.GetX() && std::abs(canvasMousePosition.GetY() - center.GetY()) <= halfSize.GetY();
		};

		if (nodes.Size() > 1)
		{
			for (ui::Node* node : nodes)
			{
				if (within(*node))
				{
					return GizmoHandle::Move;
				}
			}
			return GizmoHandle::None;
		}

		ui::Node* node = nodes[0];
		Vector2    size = node->GetSize();
		Vector2    canvasPosition = node->ComputeCanvasMatrix().GetTranslation();
		Vector2    halfSize = size * 0.5f;

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

		// The corners as soon as an axis is resizable (they resize only those), the edges of the resizable axes, the body when it can move
		const GizmoAxes axes = GetGizmoAxes(*node);

		if (axes._resizeX || axes._resizeY)
		{
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
		}

		bool withinX = canvasMousePosition.GetX() >= topLeft.GetX() && canvasMousePosition.GetX() <= topRight.GetX();
		bool withinY = canvasMousePosition.GetY() >= bottomLeft.GetY() && canvasMousePosition.GetY() <= topLeft.GetY();

		if (axes._resizeY && withinX && std::abs(canvasMousePosition.GetY() - topLeft.GetY()) <= edgeThickness)
		{
			return GizmoHandle::Top;
		}
		if (axes._resizeY && withinX && std::abs(canvasMousePosition.GetY() - bottomLeft.GetY()) <= edgeThickness)
		{
			return GizmoHandle::Bottom;
		}
		if (axes._resizeX && withinY && std::abs(canvasMousePosition.GetX() - topLeft.GetX()) <= edgeThickness)
		{
			return GizmoHandle::Left;
		}
		if (axes._resizeX && withinY && std::abs(canvasMousePosition.GetX() - topRight.GetX()) <= edgeThickness)
		{
			return GizmoHandle::Right;
		}

		if (axes._move && within(*node))
		{
			return GizmoHandle::Move;
		}

		return GizmoHandle::None;
	}

	/// @brief Drives the move/resize drag state machine and, while dragging, moves/resizes each node's rect as
	/// it was when the drag started, then places the node on it (AnchoredLayoutParams::PlaceRect: offset and
	/// size, a delta on the stretched axes). The canvas-space mouse delta is taken as is in
	/// the parents' space: the viewport draws every node axis-aligned and unscaled. Ctrl or Shift held, a click is left to the
	/// selection (adding the node under the mouse to it, or removing it) rather than starting a drag.
	/// @param nodes see GetGizmoNodes, not empty
	/// @param canvasMousePosition
	/// @param scale pixels per canvas unit
	/// @param mouseAvailable whether the mouse is over this viewport's image and usable for a new hit-test
	void UIPrefabEditorViewportWindow::UpdateGizmoDrag(const Vector<ui::Node*>& nodes, const Vector2& canvasMousePosition, float scale, bool mouseAvailable)
	{
		if (_draggedGizmoHandle != GizmoHandle::None)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
			{
				if (_gizmoMovePending)
				{
					// Released without moving: a click, selecting what is under the mouse as anywhere else (a child of the selected node...)
					UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
					tab->SetSelectedNode(PickNode(tab->GetDisplayedPrefabRoot(), _gizmoDragStartMousePosition));
				}
				_draggedGizmoHandle = GizmoHandle::None;
				_gizmoMovePending = false;
				_gizmoDragStarts.Clear();
				return;
			}

			if (_gizmoMovePending)
			{
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) == false)
				{
					return;
				}
				_gizmoMovePending = false;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				for (const GizmoDragStart& dragStart : _gizmoDragStarts)
				{
					LayoutParams* layoutParams = dragStart._node->GetLayoutParams();
					if (AnchoredLayoutParams* anchoredLayoutParams = LayoutParams::Cast<AnchoredLayoutParams>(layoutParams))
					{
						anchoredLayoutParams->SetOffset(dragStart._offset);
					}
					layoutParams->SetSize(dragStart._ownSize);
					layoutParams->SetWidthMode(dragStart._widthMode);
					layoutParams->SetHeightMode(dragStart._heightMode);
				}
				_draggedGizmoHandle = GizmoHandle::None;
				_gizmoMovePending = false;
				_gizmoDragStarts.Clear();
				GetOwner()->MarkAsDirty(); // back as it was: no undo step
				return;
			}

			Vector2     delta = canvasMousePosition - _gizmoDragStartMousePosition;
			GizmoHandle handle = _draggedGizmoHandle;
			for (const GizmoDragStart& dragStart : _gizmoDragStarts)
			{
				ui::Node*            node = dragStart._node;
				LayoutParams*         layoutParams = node->GetLayoutParams();
				AnchoredLayoutParams* anchoredLayoutParams = LayoutParams::Cast<AnchoredLayoutParams>(layoutParams);

				// The edges of the start rect, the dragged ones following the mouse (Move: all of them)
				Vector2 halfSize = dragStart._size * 0.5f;
				float   left = dragStart._center.GetX() - halfSize.GetX();
				float   right = dragStart._center.GetX() + halfSize.GetX();
				float   bottom = dragStart._center.GetY() - halfSize.GetY();
				float   top = dragStart._center.GetY() + halfSize.GetY();

				if (handle == GizmoHandle::Move || handle == GizmoHandle::Left || handle == GizmoHandle::TopLeft || handle == GizmoHandle::BottomLeft)
				{
					left += delta.GetX();
				}
				if (handle == GizmoHandle::Move || handle == GizmoHandle::Right || handle == GizmoHandle::TopRight || handle == GizmoHandle::BottomRight)
				{
					right += delta.GetX();
				}
				if (handle == GizmoHandle::Move || handle == GizmoHandle::Bottom || handle == GizmoHandle::BottomLeft || handle == GizmoHandle::BottomRight)
				{
					bottom += delta.GetY();
				}
				if (handle == GizmoHandle::Move || handle == GizmoHandle::Top || handle == GizmoHandle::TopLeft || handle == GizmoHandle::TopRight)
				{
					top += delta.GetY();
				}

				// An edge dragged past the opposite one stops on it: no negative size
				right = Max(right, left);
				top = Max(top, bottom);

				if (anchoredLayoutParams != nullptr)
				{
					Vector2 parentSize = node->GetParent() != nullptr ? node->GetParent()->GetSize() : Vector2::Zero;
					anchoredLayoutParams->PlaceRect(parentSize, Vector2((left + right) * 0.5f, (bottom + top) * 0.5f), Vector2(right - left, top - bottom));
				}
				else
				{
					// A BoxLayout's child: only its size, on the axes it owns (Fixed from then on), the layout places it
					const GizmoAxes axes = GetGizmoAxes(*node);
					Vector2         size = layoutParams->GetSize();
					if (axes._resizeX)
					{
						size.SetX(right - left);
						layoutParams->SetWidthMode(LayoutParams::SizeMode::Fixed);
					}
					if (axes._resizeY)
					{
						size.SetY(top - bottom);
						layoutParams->SetHeightMode(LayoutParams::SizeMode::Fixed);
					}
					layoutParams->SetSize(size);
				}
			}

			GetOwner()->MarkAsDirty(handle == GizmoHandle::Move ? "Move" : "Resize");
			return;
		}

		if (mouseAvailable == false || ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyShift)
		{
			_hoveredGizmoHandle = GizmoHandle::None;
			return;
		}

		_hoveredGizmoHandle = HitTestGizmoHandle(nodes, canvasMousePosition, scale);

		if (_hoveredGizmoHandle != GizmoHandle::None && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			_draggedGizmoHandle = _hoveredGizmoHandle;
			// The body covers the children: a press on it is still a click (see above) until dragged past ImGui's drag threshold
			_gizmoMovePending = _hoveredGizmoHandle == GizmoHandle::Move;
			_gizmoDragStartMousePosition = canvasMousePosition;
			_gizmoDragStarts.Clear();
			for (ui::Node* node : nodes)
			{
				LayoutParams*         layoutParams = node->GetLayoutParams();
				AnchoredLayoutParams* anchoredLayoutParams = LayoutParams::Cast<AnchoredLayoutParams>(layoutParams);

				GizmoDragStart& dragStart = _gizmoDragStarts.EmplaceBack();
				dragStart._node = node;
				dragStart._center = node->GetPosition();
				dragStart._size = node->GetSize();
				dragStart._offset = anchoredLayoutParams != nullptr ? anchoredLayoutParams->GetOffset() : Vector2::Zero;
				dragStart._ownSize = layoutParams->GetSize();
				dragStart._widthMode = layoutParams->GetWidthMode();
				dragStart._heightMode = layoutParams->GetHeightMode();
			}
		}
	}
}
