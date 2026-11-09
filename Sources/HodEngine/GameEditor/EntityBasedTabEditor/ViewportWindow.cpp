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
			bool changed = debugViewType->_drawSettings != nullptr ? DrawHelper::DrawToggleWithSettings(debugViewType->_label, debugViewType->_itemName, "DebugViewSettings", enabled)
			                                                       : DrawHelper::DrawToggle(debugViewType->_label, debugViewType->_itemName, enabled);
			if (changed)
			{
				ViewportDebugViewRegistry::SetEnabled(this, enabled ? debugViewType : nullptr);
			}
			if (debugViewType->_drawSettings != nullptr && DrawHelper::BeginSettingsPopup("DebugViewSettings"))
			{
				debugViewType->_drawSettings(*GetOwner<EntityEditorTab>()->GetWorld());
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

		// F (here or in the hierarchy) centers the view on the selected entity (its zoom kept), or with nothing selected goes back to the
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
				Entity* selection = GetOwner<EntityEditorTab>()->GetEntitySelection();
				Node2dComponent* node2d = selection != nullptr ? selection->GetComponent<Node2dComponent>() : nullptr;
				if (node2d != nullptr)
				{
					_camera.AnimateTo(node2d->GetWorldMatrix().GetTranslation(), _camera.GetZoom());
				}
				else if (selection == nullptr)
				{
					_camera.AnimateTo(Vector2::Zero, 1.0f);
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
						if (pickingId == 0)
						{
							GetOwner<EntityEditorTab>()->SetEntitySelection(nullptr);
						}
						else
						{
							game::World*  world = GetOwner<EntityEditorTab>()->GetWorld();
							Entity* pickedEntity = world->FindEntity((uint64_t)pickingId);
							if (pickedEntity != nullptr)
							{
								GetOwner<EntityEditorTab>()->SetEntitySelection(pickedEntity);
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

				Entity* sceneSelection = GetOwner<EntityEditorTab>()->GetEntitySelection();
				for (Scene* scene : world->GetScenes())
				{
					for (const auto& entityPair : scene->GetEntities())
					{
						for (Component* component : entityPair.second->GetComponents())
						{
							if (component->IsEnabledInHierarchy())
							{
								CustomComponentDrawer* drawer = CustomComponentDrawerRegistry::Find(component->GetReflectionDescriptorV());
								if (drawer != nullptr)
								{
									if (drawer->OnDrawGizmo(component, *this, entityPair.second == sceneSelection))
									{
										GetOwner()->MarkAsDirty(fmt::format("Edit {}", component->GetReflectionDescriptorV().GetDisplayName()));
									}
								}
							}
						}
					}
				}
			}
			else
			{
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

	/// @brief
	/// @param renderView
	void ViewportWindow::DrawWorld(RenderView& renderView)
	{
		game::World*       world = GetOwner<EntityEditorTab>()->GetWorld();
		ViewportDebugView* debugView = ViewportDebugViewRegistry::GetEnabled(this);
		if (debugView != nullptr)
		{
			debugView->Draw(renderView, *world);
		}
		else
		{
			ViewportWorldRenderer::Draw(renderView, *world);
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
