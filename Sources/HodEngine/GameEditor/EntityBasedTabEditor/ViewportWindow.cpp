#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Gizmos/ViewportGrid.hpp"
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Math.hpp>
#include "HodEngine/GameEditor/EntityBasedTabEditor/EntityEditorTab.hpp"
#include "HodEngine/GameEditor/PrefabEditor/PrefabAssetType.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWindow.hpp"

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Widgets.hpp>

#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>
#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/ReadbackRenderTarget.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/Texture.hpp>

#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/World.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawerRegistry.hpp"

#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportWorldRenderer.hpp"

#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/Renderer/PickingManager.hpp>
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportGamePointer.hpp"
#include <HodEngine/Core/StaticArray.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <format>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(ViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	ViewportWindow::ViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Viewport");
		SetFlags(ImGuiWindowFlags_NoScrollbar);

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
		_pickingRenderTarget = DefaultAllocator::GetInstance().New<ReadbackRenderTarget>();
	}

	/// @brief
	ViewportWindow::~ViewportWindow()
	{
		ViewportDebugViewRegistry::SetEnabled(this, nullptr);
		DefaultAllocator::GetInstance().Delete(_renderTarget);
		DefaultAllocator::GetInstance().Delete(_pickingRenderTarget);
	}

	/// @brief
	/// @return
	bool ViewportWindow::Draw()
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
			ExtendClipRectToWindowLeftEdge();
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
	void ViewportWindow::DrawContent()
	{
		// The toolbar floats over the render, which fills the whole window. It is submitted first so the viewport
		// knows when the mouse is on its buttons (no picking/zoom/pan/gizmo through them), but drawn on a channel above the image.
		ImVec2      contentPos = ImGui::GetCursorPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->ChannelsSplit(2);
		drawList->ChannelsSetCurrent(1);
		DrawToolbar();
		_mouseOverToolbar = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive();
		drawList->ChannelsSetCurrent(0);

		ImGui::SetCursorPos(contentPos);
		DrawViewport();

		drawList->ChannelsMerge();
	}

	/// @brief Toolbar floating over the top of the viewport: picking buffer, debug views (see ViewportDebugView) and grid toggles, plus the play mode resolution.
	void ViewportWindow::DrawToolbar()
	{
		struct Resolution
		{
			const char* _shortLabel;
			const char* _label;
			Vector2     _ratio;
		};

		static const Resolution resolutions[] = {
			{"16:9", "16:9 (landscape)", Vector2(16.0f, 9.0f)},
			{"21:9", "21:9 (landscape)", Vector2(21.0f, 9.0f)},
			{"9:16", "16:9 (portrait)", Vector2(9.0f, 16.0f)},
			{"9:21", "21:9 (portrait)", Vector2(9.0f, 21.0f)},
		};

		// The window itself has no padding (the viewport image goes edge to edge), keep the toolbar off the borders
		// and off the hidden tab bar triangle.
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImGui::GetStyle().WindowPadding + ImVec2(DrawHelper::GetHiddenTabBarButtonWidth(), 0.0f));

		DrawHelper::DrawToggle(ICON_MDI_EYEDROPPER " Picker", "picking buffer", _debugPicker);
		ImGui::SameLine();

		// The debug views registered by the modules, one enabled at a time
		const ViewportDebugViewType* enabledDebugViewType = ViewportDebugViewRegistry::GetEnabledType(this);
		for (const ViewportDebugViewType* debugViewType : ViewportDebugViewRegistry::GetTypes())
		{
			ImGui::PushID(debugViewType);
			bool enabled = debugViewType == enabledDebugViewType;
			if (DrawHelper::DrawToggleWithSettings(debugViewType->_label, debugViewType->_itemName, "DebugViewSettings", enabled))
			{
				ViewportDebugViewRegistry::SetEnabled(this, enabled ? debugViewType : nullptr);
			}
			if (DrawHelper::BeginSettingsPopup("DebugViewSettings"))
			{
				// Common to all the debug views: in place of the render or over it
				ImGui::TextDisabled("Display");
				if (ImGui::RadioButton("Replace the render", _debugViewMode == ViewportDebugViewMode::Replace))
				{
					_debugViewMode = ViewportDebugViewMode::Replace;
				}
				if (ImGui::RadioButton("Over the render", _debugViewMode == ViewportDebugViewMode::Overlay))
				{
					_debugViewMode = ViewportDebugViewMode::Overlay;
				}

				if (debugViewType->_drawSettings != nullptr)
				{
					ImGui::Separator();
					debugViewType->_drawSettings(*GetOwner<EntityEditorTab>()->GetWorld());
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();
			ImGui::SameLine();
		}

		DrawHelper::DrawToggleWithSettings(ICON_MDI_GRID " Grid", "grid", "ViewportGrid", _drawGrid);
		if (DrawHelper::BeginSettingsPopup("ViewportGrid"))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Cell size");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(120.0f);
			ImGui::DragFloat("##CellSize", &_gridCellSize, 0.05f, 0.01f, 10000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		const char* resolutionLabel = "Custom";
		for (const Resolution& resolution : resolutions)
		{
			if (_playRatio == resolution._ratio)
			{
				resolutionLabel = resolution._shortLabel;
			}
		}
		char resolutionButtonLabel[64];
		std::snprintf(resolutionButtonLabel, sizeof(resolutionButtonLabel), ICON_MDI_ASPECT_RATIO " %s##Resolution", resolutionLabel);
		ImVec2 resolutionPopupAnchor = ImGui::GetCursorScreenPos() + ImVec2(0.0f, ImGui::GetFrameHeight());
		if (FramedButton(resolutionButtonLabel))
		{
			ImGui::OpenPopup("ViewportResolution");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Play mode resolution");
		}

		ImGui::SetNextWindowPos(resolutionPopupAnchor);
		if (ImGui::BeginPopup("ViewportResolution"))
		{
			for (const Resolution& resolution : resolutions)
			{
				if (ImGui::Selectable(resolution._label, _playRatio == resolution._ratio))
				{
					_playRatio = resolution._ratio;
				}
			}
			ImGui::EndPopup();
		}
	}

	/// @brief
	void ViewportWindow::DrawViewport()
	{

		/*
		World* world = GetOwner<EntityEditorTab>()->GetWorld();
		if (_selectedTab != nullptr)
		{
		    world->RemoveScene(_selectedTab->_scene);
		}
		world->AddScene(tab->_scene);
		*/

		// The camera (see FramingCamera): pixels per world unit at zoom 1, the view showing DefaultCameraSize units above and below its center
		float unitScale = Max(ImGui::GetContentRegionAvail().y, 2.0f) / (2.0f * DefaultCameraSize);

		// F (here or in the hierarchy) centers the view on the selected entities (the zoom kept), or with nothing selected goes back to the
		// origin at zoom 1. Consumed even while playing: a request made then isn't for later.
		bool frameSelection = GetOwner<EntityEditorTab>()->ConsumeFrameSelectionRequest();

		if (GetOwner<EntityEditorTab>()->IsPlaying() == false || GetOwner<EntityEditorTab>()->IsPaused() == true)
		{
			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_F, false) && ImGui::GetIO().WantTextInput == false)
			{
				frameSelection = true;
			}
			if (frameSelection)
			{
				// The center of the box around the selected entities' positions (their Node2dComponent's)
				Vector<Entity*> selection = GetOwner<EntityEditorTab>()->GetSelectedEntities();
				bool            found = false;
				Vector2         min;
				Vector2         max;
				for (Entity* entity : selection)
				{
					Node2dComponent* node2d = entity->GetComponent<Node2dComponent>();
					if (node2d == nullptr)
					{
						continue;
					}
					Vector2 position = node2d->GetWorldMatrix().GetTranslation();
					min = found ? Vector2(Min(min.GetX(), position.GetX()), Min(min.GetY(), position.GetY())) : position;
					max = found ? Vector2(Max(max.GetX(), position.GetX()), Max(max.GetY(), position.GetY())) : position;
					found = true;
				}
				if (found)
				{
					_camera.AnimateTo((min + max) * 0.5f, _camera.GetZoom());
				}
				else if (selection.Empty())
				{
					_camera.AnimateTo(Vector2::Zero, 1.0f);
				}
			}

			// Delete, Ctrl+D and the clipboard's, on the selection, as in the hierarchy. Before the entities are drawn below.
			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().WantTextInput == false && ImGui::IsAnyItemActive() == false)
			{
				EntityEditorTab* tab = GetOwner<EntityEditorTab>();
				if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
				{
					tab->DeleteEntities(tab->GetSelectedSubtrees());
				}
				else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_D))
				{
					tab->DuplicateEntities(tab->GetSelectedSubtrees());
				}
				else
				{
					tab->HandleClipboardShortcuts();
				}
			}

			// The wheel zooms (eased), the middle button pans: by the mouse's move on screen, whatever the zoom
			if (ImGui::IsWindowHovered() && _mouseOverToolbar == false && ImGui::GetIO().MouseWheel != 0.0f)
			{
				_camera.ZoomBy(ImGui::GetIO().MouseWheel);
			}

			if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_T))
				{
					//_gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_R))
				{
					//_gizmoOperation = ImGuizmo::OPERATION::ROTATE_Z;
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_S))
				{
					//_gizmoOperation = ImGuizmo::OPERATION::SCALE;
				}

				if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] == true && _mouseOverToolbar == false && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
				{
					// Screen space is Y-down, world space Y-up
					float scale = unitScale * _camera.GetZoom();
					_camera.Pan(Vector2(-ImGui::GetIO().MouseDelta.x / scale, ImGui::GetIO().MouseDelta.y / scale));
				}

				if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left] == true && ImGui::IsWindowHovered() && _mouseOverToolbar == false)
				{
					ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();

					Vector2 pickingResolution = _pickingRenderTarget->GetResolution();
					if (mousePos.x >= 0 && mousePos.x < pickingResolution.GetX() && mousePos.y >= 0 && mousePos.y < pickingResolution.GetY())
					{
						ImVec2   mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
						Vector2  mousePosition(mousePos.x, mousePos.y);
						Color    pickingColor = _pickingRenderTarget->ReadPixel(mousePosition);
						uint32_t pickingId = PickingManager::ConvertColorToId(pickingColor);
						// Ctrl or Shift adds the picked entity to the selection, or removes it from it (nothing picked: the selection kept)
						bool     additive = ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyShift;
						EntityEditorTab* tab = GetOwner<EntityEditorTab>();

						// Not on a gizmo's handle (picked, but not an entity): a drag turns the click into a selection rectangle. Before the
						// click changes the selection, given back by Escape
						if (pickingId == 0 || tab->GetWorld()->FindEntity((uint64_t)pickingId) != nullptr)
						{
							ImVec2 imageSize = ImGui::GetContentRegionAvail();
							StartMarquee(ScreenToWorld(ImGui::GetIO().MousePos, ImGui::GetCursorScreenPos(), imageSize), additive);
						}

						if (pickingId == 0)
						{
							if (additive == false)
							{
								tab->SetEntitySelection(nullptr);
							}
						}
						else
						{
							// Not an entity: a gizmo's handle, left to it
							game::World* world = tab->GetWorld();
							Entity*      pickedEntity = world->FindEntity((uint64_t)pickingId);
							if (pickedEntity != nullptr)
							{
								if (additive)
								{
									tab->SetEntitySelected(pickedEntity, tab->IsSelected(pickedEntity) == false);
								}
								else
								{
									tab->SetEntitySelection(pickedEntity);
								}
							}
						}
					}
				}
			}
		}

		uint32_t resolutionWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		uint32_t resolutionHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		resolutionWidth = std::clamp(resolutionWidth, 2u, 16u * 1024u);
		resolutionHeight = std::clamp(resolutionHeight, 2u, 16u * 1024u);

		if (GetOwner<EntityEditorTab>()->IsPlaying() && GetOwner<EntityEditorTab>()->IsPaused() == false)
		{
			const float aspectRatio = _playRatio.GetX() / _playRatio.GetY();
			const float currentRatio = resolutionWidth / static_cast<float>(resolutionHeight);

			if (currentRatio > aspectRatio)
			{
				resolutionWidth = static_cast<uint32_t>(resolutionHeight * aspectRatio);
			}
			else
			{
				resolutionHeight = static_cast<uint32_t>(resolutionWidth / aspectRatio);
			}
		}

		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			_renderTarget->Init(resolutionWidth, resolutionHeight); // todo error

			_pickingRenderTarget->Init(resolutionWidth, resolutionHeight); // todo error
		}

		if (_renderTarget->IsValid() == true)
		{
			// ImVec2 origin = ImGui::GetCursorScreenPos();

			RenderView* renderView = Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
			renderView->Init();
			renderView->Prepare(_renderTarget, _pickingRenderTarget->GetRenderTarget());
			_renderView = renderView;

			if (GetOwner<EntityEditorTab>()->IsPlaying() == false || GetOwner<EntityEditorTab>()->IsPaused() == true)
			{
				Rect viewport;
				viewport._position.SetX(0);
				viewport._position.SetY(0);
				viewport._size.SetX((float)resolutionWidth);
				viewport._size.SetY((float)resolutionHeight);

				float aspect = (float)resolutionWidth / (float)resolutionHeight;

				_camera.Update(ImGui::GetIO().DeltaTime);
				float size = GetCameraSize();

				_projection = Matrix4::OrthogonalProjection(-size * aspect, size * aspect, -size, size, -1024, 1024);
				_view = Matrix4::Translation(_camera.GetPosition());

				renderView->SetupCamera(_projection, _view, viewport);

				game::World* world = GetOwner<EntityEditorTab>()->GetWorld();

				// Pushed before the world: the content of the lowest order (ZOrder 0) is then drawn over it
				if (_drawGrid)
				{
					float halfHeight = size;
					float halfWidth = size * aspect;

					ViewportGrid grid;
					grid._cellSize = _gridCellSize;
					grid._viewCenter = _camera.GetPosition();
					grid._viewHalfSize = Vector2(halfWidth, halfHeight);
					grid._pixelsPerUnit = (float)resolutionHeight / (2.0f * size);
					grid._zoom = _camera.GetZoom();
					grid._backgroundColor = renderView->GetClearColor();
					grid.Draw(*renderView);
				}

				DrawWorld(*renderView);

				// The gizmos: their handles for the active entity only (a drawer has one set of handles). Its move moves the other selected
				// entities along (see MoveSelectionAlong).
				Entity* activeEntity = GetOwner<EntityEditorTab>()->GetEntitySelection();
				Vector2 activeMove = Vector2::Zero;
				for (Scene* scene : world->GetScenes())
				{
					for (const auto& entityPair : scene->GetEntities())
					{
						bool             active = (entityPair.second == activeEntity);
						Node2dComponent* activeNode2d = active ? entityPair.second->GetComponent<Node2dComponent>() : nullptr;
						Vector2          activePosition = activeNode2d != nullptr ? activeNode2d->GetWorldMatrix().GetTranslation() : Vector2::Zero;

						for (Component* component : entityPair.second->GetComponents())
						{
							if (component->IsEnabledInHierarchy())
							{
								CustomComponentDrawer* drawer = CustomComponentDrawerRegistry::Find(component->GetReflectionDescriptorV());
								if (drawer != nullptr)
								{
									if (drawer->OnDrawGizmo(component, *this, active))
									{
										GetOwner()->MarkAsDirty(fmt::format("Edit {}", component->GetReflectionDescriptorV().GetDisplayName()));
									}
								}
							}
						}

						if (activeNode2d != nullptr)
						{
							activeMove = activeNode2d->GetWorldMatrix().GetTranslation() - activePosition;
						}
					}
				}
				if (activeMove != Vector2::Zero)
				{
					MoveSelectionAlong(activeEntity, activeMove);
				}
			}
			else
			{
				// The game's camera, whatever draws the world (a debug view replacing its render too). A default one first: the whole target, for
				// a world without the renderer's
				Rect viewport;
				viewport._position = Vector2::Zero;
				viewport._size = Vector2((float)resolutionWidth, (float)resolutionHeight);
				renderView->SetupCamera(Matrix4::Identity, Matrix4::Identity, viewport);
				ViewportWorldRenderer::SetupPlayCamera(*renderView, *GetOwner<EntityEditorTab>()->GetWorld());

				DrawWorld(*renderView);
			}

			if (_debugPicker)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - resolutionWidth) * 0.5f);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - resolutionHeight) * 0.5f);
				ImGui::Image(_pickingRenderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));
			}
			else
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - resolutionWidth) * 0.5f);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - resolutionHeight) * 0.5f);
				ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)resolutionWidth, (float)resolutionHeight));
			}

			// The selection rectangle, over the image. Not while playing: the mouse is the game's
			if (_marqueeState != MarqueeState::None)
			{
				if (GetOwner<EntityEditorTab>()->IsPlaying() && GetOwner<EntityEditorTab>()->IsPaused() == false)
				{
					_marqueeState = MarqueeState::None;
					_marqueeSelectionBefore.Clear();
				}
				else
				{
					UpdateMarquee(ImGui::GetItemRectMin(), ImGui::GetItemRectSize());
					DrawMarquee(ImGui::GetItemRectMin(), ImGui::GetItemRectSize());
				}
			}

			// Playing, the game gets the mouse over the image (see ViewportGamePointer), as the game window's
			if (GetOwner<EntityEditorTab>()->IsPlaying() && GetOwner<EntityEditorTab>()->IsPaused() == false)
			{
				ImVec2 mouseImagePos = ImGui::GetIO().MousePos - ImGui::GetItemRectMin();
				bool   mouseOverImage = ImGui::IsItemHovered() && _mouseOverToolbar == false && mouseImagePos.x >= 0.0f && mouseImagePos.y >= 0.0f &&
				                      mouseImagePos.x < (float)resolutionWidth && mouseImagePos.y < (float)resolutionHeight;
				if (mouseOverImage && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					_gamePointerDown = true;
				}
				else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
				{
					_gamePointerDown = false;
				}

				if (mouseOverImage || _gamePointerDown)
				{
					// The wheel over the image only (ImGui's horizontal one is > 0 towards the left)
					Vector2 scroll = mouseOverImage ? Vector2(-ImGui::GetIO().MouseWheelH, ImGui::GetIO().MouseWheel) : Vector2::Zero;
					ViewportGamePointer::Dispatch(Vector2(mouseImagePos.x, mouseImagePos.y), Vector2((float)resolutionWidth, (float)resolutionHeight), _gamePointerDown,
					                              scroll);
				}
			}
			else
			{
				_gamePointerDown = false;
			}
			// ImGui::GetWindowDrawList()->AddImage(_renderTarget->GetColorTexture(), origin + ImVec2(0.0f, (float)menuBarHeight), origin + ImVec2((float)windowWidth,
			// (float)(windowHeight + menuBarHeight)));
			if (ImGui::BeginDragDropTarget() == true)
			{
				const ImGuiPayload* payload = ImGui::GetDragDropPayload();
				if (payload->IsDataType("FileSystemMapping") == true)
				{
					payload = ImGui::AcceptDragDropPayload("FileSystemMapping");
					if ((payload) != nullptr)
					{
						// todo factorize
						AssetDatabase::FileSystemMapping* node = *static_cast<AssetDatabase::FileSystemMapping**>(payload->Data);
						std::shared_ptr<Asset>            asset = node->_asset;
						if (asset != nullptr)
						{
							// Under the root entity if any (a prefab's), not into the prefab itself
							EntityEditorTab* tab = GetOwner<EntityEditorTab>();
							if (asset->GetType() == PrefabAssetType::GetHash() && tab->CanInstantiatePrefab(asset->GetUid()))
							{
								std::shared_ptr<PrefabResource> prefabResource = ResourceManager::GetInstance()->GetResource<PrefabResource>(asset->GetUid());

								Entity* instance = tab->GetCurrentScene()->Instantiate(prefabResource);
								if (instance != nullptr && tab->GetDefaultParent() != nullptr)
								{
									instance->SetParent(tab->GetDefaultParent());
								}

								tab->SetEntitySelection(instance);
								GetOwner()->MarkAsDirty(fmt::format("Add {}", asset->GetName()));
							}
							else if (asset->GetType() == PrefabAssetType::GetHash())
							{
								OUTPUT_ERROR("A prefab can't contain itself: {} holds an instance of it", asset->GetName());
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			_renderView = nullptr;
		}
	}

	/// @brief The view's camera: the world position under screenPosition
	/// @param screenPosition
	/// @param imagePosition screen position of the image's top-left corner
	/// @param imageSize
	/// @return
	Vector2 ViewportWindow::ScreenToWorld(const ImVec2& screenPosition, const ImVec2& imagePosition, const ImVec2& imageSize) const
	{
		// Screen space is Y-down, world space Y-up
		float scale = Max(imageSize.y, 1.0f) / (2.0f * GetCameraSize());
		return _camera.GetPosition() + Vector2((screenPosition.x - imagePosition.x - imageSize.x * 0.5f) / scale,
		                                       -(screenPosition.y - imagePosition.y - imageSize.y * 0.5f) / scale);
	}

	/// @brief The inverse of ScreenToWorld
	/// @param worldPosition
	/// @param imagePosition
	/// @param imageSize
	/// @return
	ImVec2 ViewportWindow::WorldToScreen(const Vector2& worldPosition, const ImVec2& imagePosition, const ImVec2& imageSize) const
	{
		float   scale = Max(imageSize.y, 1.0f) / (2.0f * GetCameraSize());
		Vector2 relative = worldPosition - _camera.GetPosition();
		return ImVec2(imagePosition.x + imageSize.x * 0.5f + relative.GetX() * scale, imagePosition.y + imageSize.y * 0.5f - relative.GetY() * scale);
	}

	/// @brief A left press on the image, not on a gizmo's handle: still a click until dragged past ImGui's drag threshold
	/// @param worldPosition where it starts, in world space: the rectangle stays on the content while the view pans or zooms
	/// @param additive Ctrl or Shift held: the entities inside are added to the selection from before the press
	void ViewportWindow::StartMarquee(const Vector2& worldPosition, bool additive)
	{
		_marqueeState = MarqueeState::Pending;
		_marqueeStart = worldPosition;
		_marqueeAdditive = additive;
		_marqueeSelectionBefore.Clear();
		for (Entity* entity : GetOwner<EntityEditorTab>()->GetSelectedEntities())
		{
			_marqueeSelectionBefore.PushBack(entity);
		}
		_marqueeLastRect = ImVec4(0.0f, 0.0f, -1.0f, -1.0f);
	}

	/// @brief Follows the press started by StartMarquee: once dragged, the selection is the entities drawn inside the rectangle (see
	/// CollectEntitiesInRect), after the selection from before the press when additive. Recomputed from the press' selection whenever the
	/// rectangle changes, so shrinking it deselects again. Ends on release, Escape gives back the selection from before the press.
	/// @param imagePosition screen position of the image's top-left corner
	/// @param imageSize
	void ViewportWindow::UpdateMarquee(const ImVec2& imagePosition, const ImVec2& imageSize)
	{
		EntityEditorTab* tab = GetOwner<EntityEditorTab>();

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

		Vector<Entity*> selectionBefore;
		for (const WeakPtr<Entity>& entity : _marqueeSelectionBefore)
		{
			if (entity.Get() != nullptr)
			{
				selectionBefore.PushBack(entity.Get());
			}
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
		{
			if (_marqueeState == MarqueeState::Active)
			{
				tab->SetSelectedEntities(selectionBefore);
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

		// In the image's pixels, clamped to it
		ImVec2 start = WorldToScreen(_marqueeStart, imagePosition, imageSize) - imagePosition;
		ImVec2 end = ImGui::GetIO().MousePos - imagePosition;
		ImVec4 rect(Clamp(Min(start.x, end.x), 0.0f, imageSize.x), Clamp(Min(start.y, end.y), 0.0f, imageSize.y), Clamp(Max(start.x, end.x), 0.0f, imageSize.x),
		            Clamp(Max(start.y, end.y), 0.0f, imageSize.y));
		if (rect.x == _marqueeLastRect.x && rect.y == _marqueeLastRect.y && rect.z == _marqueeLastRect.z && rect.w == _marqueeLastRect.w)
		{
			return; // Nothing new to read in the picking buffer
		}
		_marqueeLastRect = rect;

		Vector<Entity*> selection;
		if (_marqueeAdditive)
		{
			selection = selectionBefore;
		}
		for (Entity* entity : CollectEntitiesInRect(Vector2(rect.x, rect.y), Vector2(rect.z, rect.w)))
		{
			if (std::find(selection.Begin(), selection.End(), entity) == selection.End())
			{
				selection.PushBack(entity);
			}
		}
		tab->SetSelectedEntities(selection);
	}

	/// @brief The rectangle, over the image (ImGui, in screen space: crisp at any zoom), from its world space start to the mouse
	/// @param imagePosition
	/// @param imageSize
	void ViewportWindow::DrawMarquee(const ImVec2& imagePosition, const ImVec2& imageSize)
	{
		if (_marqueeState != MarqueeState::Active)
		{
			return;
		}

		ImVec2 start = WorldToScreen(_marqueeStart, imagePosition, imageSize);
		ImVec2 end = ImGui::GetIO().MousePos;
		ImVec2 min(Min(start.x, end.x), Min(start.y, end.y));
		ImVec2 max(Max(start.x, end.x), Max(start.y, end.y));

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(imagePosition, imagePosition + imageSize, true);
		drawList->AddRectFilled(min, max, IM_COL32(120, 170, 255, 40));
		drawList->AddRect(min, max, IM_COL32(120, 170, 255, 220));
		drawList->PopClipRect();
	}

	/// @brief The entities with pixels inside [min, max] (image pixels) in the picking buffer (as the GPU last finished it, a few frames
	/// ago), in the order they are met: whatever draws them (sprites, texts, gizmos of their components...), an entity drawing nothing
	/// can't be met. One pixel out of two on each axis is read.
	/// @param min
	/// @param max
	/// @return
	Vector<Entity*> ViewportWindow::CollectEntitiesInRect(const Vector2& min, const Vector2& max) const
	{
		Vector<Entity*> entities;
		Vector2         resolution = _pickingRenderTarget->GetResolution();
		game::World*    world = const_cast<ViewportWindow*>(this)->GetOwner<EntityEditorTab>()->GetWorld();

		constexpr uint32_t step = 2;
		uint32_t           lastPickingId = 0;
		Vector<uint32_t>   seenPickingIds;
		uint32_t xBegin = (uint32_t)Max(min.GetX(), 0.0f);
		uint32_t yBegin = (uint32_t)Max(min.GetY(), 0.0f);
		uint32_t xEnd = (uint32_t)Min(max.GetX(), resolution.GetX());
		uint32_t yEnd = (uint32_t)Min(max.GetY(), resolution.GetY());
		for (uint32_t y = yBegin; y < yEnd; y += step)
		{
			for (uint32_t x = xBegin; x < xEnd; x += step)
			{
				uint32_t pickingId = PickingManager::ConvertColorToId(_pickingRenderTarget->ReadPixel(Vector2((float)x, (float)y)));
				if (pickingId == 0 || pickingId == lastPickingId)
				{
					continue;
				}
				lastPickingId = pickingId;
				if (std::find(seenPickingIds.Begin(), seenPickingIds.End(), pickingId) != seenPickingIds.End())
				{
					continue;
				}
				seenPickingIds.PushBack(pickingId);

				// Not an entity: a gizmo's handle
				if (Entity* entity = world->FindEntity((uint64_t)pickingId))
				{
					entities.PushBack(entity);
				}
			}
		}
		return entities;
	}

	/// @brief The active entity was moved by its gizmo: the other selected entities follow, by the same move in world space. Not those
	/// inside it or holding it (they move with it already, or move it), nor those without a Node2dComponent.
	/// @param activeEntity
	/// @param worldMove
	void ViewportWindow::MoveSelectionAlong(Entity* activeEntity, const Vector2& worldMove)
	{
		auto isInside = [](const Entity* entity, const Entity* ancestor)
		{
			for (; entity != nullptr; entity = entity->GetParent().Lock())
			{
				if (entity == ancestor)
				{
					return true;
				}
			}
			return false;
		};

		for (Entity* entity : GetOwner<EntityEditorTab>()->GetSelectedSubtrees())
		{
			if (isInside(entity, activeEntity) || isInside(activeEntity, entity))
			{
				continue;
			}
			Node2dComponent* node2d = entity->GetComponent<Node2dComponent>();
			if (node2d == nullptr)
			{
				continue;
			}

			// Its local position giving its new world one, through its parent's matrix (its world one without its own local one)
			Matrix4 parentMatrix = node2d->GetWorldMatrix() * Matrix4::Inverse(node2d->GetLocalMatrix());
			Vector2 worldPosition = node2d->GetWorldMatrix().GetTranslation() + worldMove;
			Vector4 localPosition = Matrix4::Inverse(parentMatrix) * Vector4(worldPosition.GetX(), worldPosition.GetY(), 0.0f, 1.0f);
			node2d->SetPosition(Vector2(localPosition.GetX(), localPosition.GetY()));
		}
	}

	/// @brief
	/// @param renderView
	void ViewportWindow::DrawWorld(RenderView& renderView)
	{
		game::World*       world = GetOwner<EntityEditorTab>()->GetWorld();
		ViewportDebugView* debugView = ViewportDebugViewRegistry::GetEnabled(this);
		if (debugView == nullptr)
		{
			ViewportWorldRenderer::Draw(renderView, *world);
		}
		else if (_debugViewMode == ViewportDebugViewMode::Overlay)
		{
			ViewportWorldRenderer::Draw(renderView, *world);
			debugView->Draw(renderView, *world, GizmoViewport::OverlayOrder - 1); // under the gizmos' outlines and handles
		}
		else
		{
			debugView->Draw(renderView, *world, 0);
		}
	}

	/// @brief
	/// @return
	ReadbackRenderTarget* ViewportWindow::GetPickingRenderTarget() const
	{
		return _pickingRenderTarget;
	}

	/// @brief
	/// @return
	const Matrix4& ViewportWindow::GetProjectionMatrix() const
	{
		return _projection;
	}

	/// @brief
	/// @return
	const Matrix4& ViewportWindow::GetViewMatrix() const
	{
		return _view;
	}

	/// @brief
	/// @return
	const Vector2& ViewportWindow::GetPlayRatio() const
	{
		return _playRatio;
	}

	/// @brief
	/// @return
	bool ViewportWindow::IsMouseOverToolbar() const
	{
		return _mouseOverToolbar;
	}

	float ViewportWindow::GetCameraSize() const
	{
		return DefaultCameraSize / _camera.GetZoom();
	}

	RenderView* ViewportWindow::GetRenderView()
	{
		return _renderView;
	}
}
