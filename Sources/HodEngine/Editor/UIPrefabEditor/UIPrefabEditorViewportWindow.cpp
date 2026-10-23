#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Widgets.hpp>

#include <HodEngine/Core/Math.hpp>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/BoxLayout.hpp>
#include <HodEngine/UI2/BoxLayoutParams.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/Texture.hpp>

#include <algorithm>
#include "HodEngine/Core/StaticArray.hpp"
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
		ui2::Node*         root = tab->GetCanvas().GetRootNode();

		// Laid out for the previewed device as it would be in game. Only the tab's runtime canvas state: never saved in the prefab.
		// Eased towards the device's resolution rather than snapping to it, so switching device (or rotating it) morphs the layout progressively.
		Vector2 previewGoal = _settings.GetPreviewResolution();
		if (previewGoal == Vector2::Zero)
		{
			previewGoal = tab->GetCanvas().GetScaler()._designResolution;
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
		tab->GetCanvas().SetTargetResolution(_previewResolution);
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

		bool viewportResized = false;
		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			Texture::CreateInfo createInfo;

			createInfo._allowReadWrite = false;
			_renderTarget->Init(resolutionWidth, resolutionHeight, createInfo); // todo error
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
		tab->GetCanvas().GetScaler().Compute(previewGoal, goalLayoutSize, goalScaleFactor);
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
		if (hovered && ImGui::GetIO().MouseWheel != 0.0f)
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
		// Delete (here or in the hierarchy) removes the selected node. Before the selection is read below, nothing of it is drawn yet.
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_Delete, false) && ImGui::GetIO().WantTextInput == false &&
		    tab->GetSelectedNode() != nullptr && _draggedGizmoHandle == GizmoHandle::None)
		{
			tab->DeleteNode(tab->GetSelectedNode());
		}

		bool frameDevice = _frameDeviceRequested;
		_frameDeviceRequested = false;

		Vector2    viewSize((float)resolutionWidth, (float)resolutionHeight);
		ui2::Node* selectedNode = tab->GetSelectedNode();
		if (frameSelection && frameDevice == false && selectedNode != nullptr)
		{
			_camera.Frame(selectedNode->ComputeCanvasMatrix().GetTranslation(), selectedNode->GetSize(), viewSize, baseScale, true);
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
			ui2::Node* prefabRoot = tab->GetPrefabRoot();
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
		if (_settings._gridVisible)
		{
			DrawGrid(gridOrigin, _camera.GetPosition(), worldHalfWidth, worldHalfHeight, scale, *renderView);
		}
		DrawPreviewFrames(*renderView);

		//Gizmos::Rect(Matrix4::Identity, canvasSize, Color(90.0f / 255.0f, 90.0f / 255.0f, 90.0f / 255.0f, 1.0f), *renderView);

		// The actual content first: the outlines/gizmos below are pushed with a later (or the maximum)
		// rendering order, so they stay on top of it.
		tab->GetCanvas().PushRenderCommand(*renderView, RenderView::RenderQueueType::World);

		// Only the prefab root's subtree: `root` is the hidden container, whose rect is the design resolution, not part of the prefab.
		DrawNode(tab->GetPrefabRoot(), *renderView);

		if (selectedNode != nullptr)
		{
			DrawSpacing(selectedNode, *renderView);
		}

		if (selectedNode != nullptr && LayoutParams::Cast<AnchoredLayoutParams>(selectedNode->GetLayoutParams()) != nullptr)
		{
			DrawSelectionGizmo(selectedNode, scale, *renderView);
		}

		ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));
		DrawPreviewLabels(imagePos, ImVec2((float)resolutionWidth, (float)resolutionHeight), scale);

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
		const ui2::CanvasScaler& scaler = tab->GetCanvas().GetScaler();

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
			ImGui::SetTooltip("Edited in the Canvas Scaler (select the Canvas in the hierarchy)");
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

	/// @brief Size, in canvas units, of the previewed screen (at its current, possibly still easing, resolution): the canvas'
	/// layout size, except in CanvasScaler::Mode::Fit where the screen can be larger than the (design sized) canvas, the
	/// difference being the letterbox bars.
	Vector2 UIPrefabEditorViewportWindow::GetDeviceFrameSize() const
	{
		const ui2::Canvas& canvas = const_cast<UIPrefabEditorViewportWindow*>(this)->GetOwner<UIPrefabEditorTab>()->GetCanvas();
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
		Gizmos::Rect(Matrix4::Identity, GetDeviceFrameSize(), Color(0.85f, 0.85f, 0.85f, 1.0f), renderView);
	}

	/// @brief Labels the frame of DrawPreviewFrames with the device's name and resolution, above its top-left corner.
	/// Done with ImGui over the image (the render target can't draw text), hence the manual canvas -> screen projection,
	/// the inverse of DrawViewport's screenToCanvas.
	/// @param imagePos screen position of the image's top-left corner
	/// @param imageSize
	/// @param scale pixels per canvas unit
	void UIPrefabEditorViewportWindow::DrawPreviewLabels(const ImVec2& imagePos, const ImVec2& imageSize, float scale)
	{
		const ui2::CanvasScaler& scaler = GetOwner<UIPrefabEditorTab>()->GetCanvas().GetScaler();

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
		float zoomLevel = -std::log2(std::max(_camera.GetZoom(), 1e-6f));
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

	/// @brief The selected node's spacing, filled as in a browser's inspector: its padding (inside its rect, around its children)
	/// in green when it has a BoxLayout, its margin (outside its rect, pushing its neighbours away) in orange when its parent has one.
	/// Axis-aligned, as the outlines (see DrawNode).
	/// @param node
	/// @param renderView
	void UIPrefabEditorViewportWindow::DrawSpacing(ui2::Node* node, RenderView& renderView)
	{
		Rect rect;
		rect._size = node->GetSize();
		rect._position = node->ComputeCanvasMatrix().GetTranslation() - rect._size * 0.5f;

		if (const ui2::BoxLayout* boxLayout = node->GetLayout<ui2::BoxLayout>())
		{
			Rect inner = rect.Inset(boxLayout->GetPadding());
			Gizmos::FilledFrame(Matrix4::Identity, rect._position, rect._size, inner._position, inner._size, Color(147.0f / 255.0f, 196.0f / 255.0f, 125.0f / 255.0f, 0.55f),
			                    renderView);
		}

		if (const ui2::BoxLayoutParams* boxLayoutParams = LayoutParams::Cast<ui2::BoxLayoutParams>(node->GetLayoutParams()))
		{
			const ui2::Margin& margin = boxLayoutParams->GetMargin();
			Rect               outer;
			outer._position = rect._position - Vector2(margin._left, margin._bottom);
			outer._size = rect._size + Vector2(margin._left + margin._right, margin._bottom + margin._top);
			Gizmos::FilledFrame(Matrix4::Identity, outer._position, outer._size, rect._position, rect._size, Color(246.0f / 255.0f, 178.0f / 255.0f, 107.0f / 255.0f, 0.55f),
			                    renderView);
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
			StaticArray<Vector2, 4> vertices = {
				center + Vector2(-halfSize.GetX(), halfSize.GetY()),
				center + Vector2(halfSize.GetX(), halfSize.GetY()),
				center + Vector2(halfSize.GetX(), -halfSize.GetY()),
				center + Vector2(-halfSize.GetX(), -halfSize.GetY()),
			};
			StaticArray<uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};

			MaterialInstance* materialInstance = MaterialInstance::Create(
				BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
			materialInstance->SetVec4("ubo.color", Vector4(color.r, color.g, color.b, color.a));

			RenderCommandMesh* command = DefaultAllocator::GetInstance().New<RenderCommandMesh>(
				vertices.Data(), nullptr, nullptr, (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), Matrix4::Identity, materialInstance,
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

	/// @brief Drives the move/resize drag state machine and, while dragging, moves/resizes the node's rect as
	/// it was when the drag started, then places the node on it (AnchoredLayoutParams::PlaceRect: offset and
	/// size, a delta on the stretched axes). The canvas-space mouse delta is taken as is in
	/// the parent's space: the viewport draws every node axis-aligned and unscaled.
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
				layoutParams->SetSize(_gizmoDragStartOwnSize);
				layoutParams->SetWidthMode(_gizmoDragStartWidthMode);
				layoutParams->SetHeightMode(_gizmoDragStartHeightMode);
				_draggedGizmoHandle = GizmoHandle::None;
				GetOwner()->MarkAsDirty();
				return;
			}

			Vector2 delta = canvasMousePosition - _gizmoDragStartMousePosition;

			// The edges of the start rect, the dragged ones following the mouse (Move: all of them)
			Vector2 halfSize = _gizmoDragStartSize * 0.5f;
			float   left = _gizmoDragStartCenter.GetX() - halfSize.GetX();
			float   right = _gizmoDragStartCenter.GetX() + halfSize.GetX();
			float   bottom = _gizmoDragStartCenter.GetY() - halfSize.GetY();
			float   top = _gizmoDragStartCenter.GetY() + halfSize.GetY();

			GizmoHandle handle = _draggedGizmoHandle;
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

			Vector2 parentSize = node->GetParent() != nullptr ? node->GetParent()->GetSize() : Vector2::Zero;
			layoutParams->PlaceRect(parentSize, Vector2((left + right) * 0.5f, (bottom + top) * 0.5f), Vector2(right - left, top - bottom));

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
			_gizmoDragStartCenter = node->GetPosition();
			_gizmoDragStartSize = node->GetSize();
			_gizmoDragStartOffset = layoutParams->GetOffset();
			_gizmoDragStartOwnSize = layoutParams->GetSize();
			_gizmoDragStartWidthMode = layoutParams->GetWidthMode();
			_gizmoDragStartHeightMode = layoutParams->GetHeightMode();
		}
	}
}
