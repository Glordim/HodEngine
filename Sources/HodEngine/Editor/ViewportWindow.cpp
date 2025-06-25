#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"
#include "HodEngine/Editor/Importer/PrefabImporter.hpp"
#include "HodEngine/Editor/SceneEditor/SceneEditorTab.hpp"

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Core/Rect.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>
#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Core/Resource/ResourceManager.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

#include "HodEngine/Editor/PhysicsDebugDrawer.hpp"
#include <HodEngine/Physics/World.hpp>
#include <HodEngine/Physics/DebugDrawer.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

#include <HodEngine/Renderer/PickingManager.hpp>

#include <cmath>
#include <format>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(ViewportWindow, "Viewport", false)

	/// @brief 
	ViewportWindow::ViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);

		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_pickingRenderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_renderView.Init();
	}

	/// @brief 
	ViewportWindow::~ViewportWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
		DefaultAllocator::GetInstance().Delete(_pickingRenderTarget);
	}

	/// @brief 
	/// @return 
	bool ViewportWindow::Draw()
	{
		bool open = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		bool visible = ImGui::Begin(GetIdentifier(), &open, GetFlags());
		ImGui::PopStyleVar();
		if (visible)
		{
			ImRect cliprect = ImGui::GetCurrentWindow()->ClipRect;
			cliprect.Min.x -= ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Min.y -= ImGui::GetStyle().WindowPadding.y * 0.5f;
			cliprect.Max.x += ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Max.y += ImGui::GetStyle().WindowPadding.y * 0.5f;
			ImGui::PopClipRect();
			ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);
			DrawContent();
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
		if (ImGui::BeginMenuBar())
		{
			ImGui::MenuItem("Debug Picker", nullptr, &_debugPicker);
			ImGui::SameLine();
			bool enabled = _physicsDebugDrawer != nullptr;
			if (ImGui::MenuItem("Debug Physics", nullptr, &enabled) == true)
			{
				EnablePhysicsDebugDrawer(enabled);
			}
			ImGui::SameLine();
			if (ImGui::BeginMenu(ICON_MDI_TRIANGLE_SMALL_DOWN))
			{
				game::World* world = GetOwner<EntityEditorTab>()->GetWorld();
				physics::World* physicsWorld = world->GetPhysicsWorld();
				physics::DebugDrawer* physicsDebugDrawer = physicsWorld->GetDebugDrawer();
				uint32_t flags = physicsDebugDrawer->GetFlags();

				for (const physics::DebugDrawer::Flag& flag : physicsDebugDrawer->GetAvailableFlags())
				{
					bool enabled = flags & flag._value;
					if (ImGui::MenuItem(flag._label, nullptr, &enabled))
					{
						if (enabled)
						{
							flags |= flag._value;
						}
						else
						{
							flags &= ~(flag._value);
						}
						physicsDebugDrawer->SetFlags(flags);
					}
				}
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Resolution"))
			{
				if (ImGui::MenuItem("16:9 (landscapce)"))
				{
					_playRatio = Vector2(16.0f, 9.0f);
				}
				if (ImGui::MenuItem("21:9 (landscapce)"))
				{
					_playRatio = Vector2(21.0f, 9.0f);
				}
				if (ImGui::MenuItem("16:9 (portrait)"))
				{
					_playRatio = Vector2(9.0f, 16.0f);
				}
				if (ImGui::MenuItem("21:9 (portrait)"))
				{
					_playRatio = Vector2(9.0f, 21.0f);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		/*
		game::World* world = GetOwner<EntityEditorTab>()->GetWorld();
		if (_selectedTab != nullptr)
		{
			world->RemoveScene(_selectedTab->_scene);
		}
		world->AddScene(tab->_scene);
		*/

		if (GetOwner<EntityEditorTab>()->IsPlaying() == false || GetOwner<EntityEditorTab>()->IsPaused() == true)
		{
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

				if (ImGui::GetIO().MouseWheel != 0.0f)
				{
					_size -= ImGui::GetIO().MouseWheel * 0.1f * std::abs(_size);
				}

				if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] == true && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
				{
					Vector2 movement(ImGui::GetIO().MouseDelta.x * 0.01f, -ImGui::GetIO().MouseDelta.y * 0.01f);
					_cameraPosition.SetX(_cameraPosition.GetX() - movement.GetX());
					_cameraPosition.SetY(_cameraPosition.GetY() - movement.GetY());
				}

				if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left] == true && ImGui::IsWindowHovered())
				{
					ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();

					Vector2 pickingResolution = _pickingRenderTarget->GetResolution();
					if (mousePos.x >= 0 && mousePos.x < pickingResolution.GetX() && mousePos.y >= 0 && mousePos.y < pickingResolution.GetY())
					{
						ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
						Vector2 mousePosition(mousePos.x, mousePos.y);
						Color pickingColor = _pickingRenderTarget->GetColorTexture()->ReadPixel(mousePosition);
						uint32_t pickingId = renderer::PickingManager::ConvertColorToId(pickingColor);
						if (pickingId == 0)
						{
							GetOwner<EntityEditorTab>()->SetEntitySelection(nullptr);
						}
						else
						{
							game::World* world = GetOwner<EntityEditorTab>()->GetWorld();
							game::Entity* pickedEntity = world->FindEntity((uint64_t)pickingId);
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
				resolutionWidth = static_cast<uint32_t>(resolutionHeight * aspectRatio);
			else
				resolutionHeight = static_cast<uint32_t>(resolutionWidth / aspectRatio);
		}

		if (_renderTarget->GetResolution().GetX() != resolutionWidth ||
			_renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			renderer::Texture::CreateInfo createInfo;

			createInfo._allowReadWrite = false;
			_renderTarget->Init(resolutionWidth, resolutionHeight, createInfo); // todo error

			createInfo._allowReadWrite = true;
			_pickingRenderTarget->Init(resolutionWidth, resolutionHeight, createInfo); // todo error
		}

		if (_renderTarget->IsValid() == true)
		{
			//ImVec2 origin = ImGui::GetCursorScreenPos();

			_renderView.Prepare(_renderTarget, _pickingRenderTarget);

			if (GetOwner<EntityEditorTab>()->IsPlaying() == false || GetOwner<EntityEditorTab>()->IsPaused() == true)
			{
				Rect viewport;
				viewport._position.SetX(0);
				viewport._position.SetY(0);
				viewport._size.SetX((float)resolutionWidth);
				viewport._size.SetY((float)resolutionHeight);

				float aspect = (float)resolutionWidth / (float)resolutionHeight;

				_projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
				_view = Matrix4::Translation(_cameraPosition);

				_renderView.SetupCamera(_projection, _view, viewport);

				game::World* world = GetOwner<EntityEditorTab>()->GetWorld();
				if (_physicsDebugDrawer != nullptr)
				{
					_physicsDebugDrawer->Update(world->GetPhysicsWorld());
					_physicsDebugDrawer->PushRenderCommand(_renderView, world->GetPhysicsWorld());
				}
				else
				{
					world->Draw(_renderView);
				}

				game::Entity* sceneSelection = GetOwner<EntityEditorTab>()->GetEntitySelection();
				for (game::Scene* scene : world->GetScenes())
				{
					for (const auto& entityPair : scene->GetEntities())
					{
						for (game::Component* component : entityPair.second->GetComponents())
						{
							if (component->IsEnabledInHierarchy())
							{
								ReflectionTraitComponentCustomEditor* customEditorTrait = component->GetReflectionDescriptorV().FindTrait<ReflectionTraitComponentCustomEditor>();
								if (customEditorTrait != nullptr)
								{
									ComponentCustomEditor* customEditor = customEditorTrait->GetCustomEditor();
									if (customEditor != nullptr)
									{
										if (customEditor->OnDrawGizmo(component, *this, (entityPair.second == sceneSelection)))
										{
											GetOwner()->MarkAssetAsDirty();
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				game::World* world = GetOwner<EntityEditorTab>()->GetWorld();
				if (_physicsDebugDrawer != nullptr)
				{
					_physicsDebugDrawer->Update(world->GetPhysicsWorld());
					_physicsDebugDrawer->PushRenderCommand(_renderView, world->GetPhysicsWorld());
				}
				else
				{
					world->Draw(_renderView);
				}
			}

			renderer::Renderer::GetInstance()->PushRenderView(_renderView, false);

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
			//ImGui::GetWindowDrawList()->AddImage(_renderTarget->GetColorTexture(), origin + ImVec2(0.0f, (float)menuBarHeight), origin + ImVec2((float)windowWidth, (float)(windowHeight + menuBarHeight)));
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
						std::shared_ptr<Asset> asset = node->_asset;
						if (asset != nullptr)
						{
							PrefabImporter prefabImporter;
							if (asset->GetMeta()._importerType == prefabImporter.GetTypeName())
							{
								std::shared_ptr<game::PrefabResource> prefabResource = ResourceManager::GetInstance()->GetResource<game::PrefabResource>(asset->GetUid());

								GetOwner<EntityEditorTab>()->GetCurrentScene()->Instantiate(prefabResource);

								GetOwner()->MarkAssetAsDirty();
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	}

	/// @brief 
	/// @param enabled 
	void ViewportWindow::EnablePhysicsDebugDrawer(bool enabled)
	{
		if (enabled == true)
		{
			if (_physicsDebugDrawer == nullptr)
			{
				_physicsDebugDrawer = DefaultAllocator::GetInstance().New<PhysicsDebugDrawer>();
			}
		}
		else
		{
			if (_physicsDebugDrawer != nullptr)
			{
				DefaultAllocator::GetInstance().Delete(_physicsDebugDrawer);
				_physicsDebugDrawer = nullptr;
			}
		}
	}

	/// @brief 
	/// @param enabled 
	/// @return 
	bool ViewportWindow::IsPhysicsDebugDrawerEnabled(bool enabled) const
	{
		return (_physicsDebugDrawer != nullptr);
	}

	/// @brief 
	/// @return 
	renderer::RenderView* ViewportWindow::GetRenderView()
	{
		return &_renderView;
	}

	/// @brief 
	/// @return 
	renderer::RenderTarget* ViewportWindow::GetPickingRenderTarget() const
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

	float ViewportWindow::GetCameraSize() const
	{
		return _size;
	}
}
