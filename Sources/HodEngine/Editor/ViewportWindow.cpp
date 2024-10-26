#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"
#include "HodEngine/Editor/Importer/PrefabImporter.hpp"

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Core/Rect.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>
#include <HodEngine/Core/Color.hpp>

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

#include "HodEngine/Editor/PhysicsDebugDrawer.hpp"
#include <HodEngine/Physics/Physics.hpp>
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
	ViewportWindow::ViewportWindow()
	{
		SetFlags(ImGuiWindowFlags_MenuBar);

		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_pickingRenderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_renderQueue.Init();

		_scene = new game::Scene();
		SetId(reinterpret_cast<uint64_t>(_scene));

		game::World* world = game::World::GetInstance();
		world->AddScene(_scene);

		SetTitle("New Scene");
	}

	/// @brief 
	ViewportWindow::ViewportWindow(std::shared_ptr<Asset> asset)
	{
		_asset = asset;
		SetTitle(asset->GetName());

		SetFlags(ImGuiWindowFlags_MenuBar);

		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_pickingRenderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_renderQueue.Init();

		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, asset->GetPath()) == false)
		{
			return; // todo message + bool
		}

		_scene = new game::Scene();
		_scene->SetName(asset->GetName());
		SetId(reinterpret_cast<uint64_t>(_scene));

		SceneImporter sceneImporter;
		if (asset->GetMeta()._importerType == sceneImporter.GetTypeName())
		{
			if (Serializer::Deserialize(_scene, document.GetRootNode()) == false)
			{
				return; // todo message + bool
			}
			asset->SetInstanceToSave(_scene, _scene->GetReflectionDescriptorV());
		}
		else
		{
			game::Prefab* prefab = new game::Prefab();
			if (Serializer::Deserialize(prefab, document.GetRootNode()) == false)
			{
				return; // todo message + bool
			}
			std::shared_ptr<game::Entity> prefabRootEntity = _scene->Instantiate(*prefab);
			if (prefabRootEntity != nullptr) // TODO a Prefab should not be empty
			{
				prefabRootEntity->SetPrefab(nullptr); // Unpack prefab for serialization, otherwise that will be serialize as PrefabInstance
			}
			delete prefab;
			asset->SetInstanceToSave(_scene, _scene->GetReflectionDescriptorV());
		}
	}

	/// @brief 
	ViewportWindow::~ViewportWindow()
	{
		game::World* world = game::World::GetInstance();
		world->RemoveScene(_scene);

		if (_asset != nullptr)
		{
			_asset->SetInstanceToSave(nullptr, nullptr);
			_asset->ResetDirty();
		}
		delete _scene;

		delete _renderTarget;
		delete _pickingRenderTarget;
	}

	/// @brief 
	void ViewportWindow::ReloadScene()
	{
		game::World* world = game::World::GetInstance();
		world->RemoveScene(_scene);
		world->AddScene(_scene);
	}

	bool ViewportWindow::Draw()
	{
		bool open = true;

		// todo override GetIdentifier ?
		if (_asset != nullptr && _assetWasDirty == true && _asset->IsDirty() == false)
		{
			_assetWasDirty = false;
			SetTitle(_asset->GetName());
		}
		//

		ImGui::SetNextWindowDockID(imgui::ImGuiManager::GetInstance()->GetCentralDockSpace(), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		bool beginResult = ImGui::Begin(GetIdentifier(), &open, GetFlags());
		ImGui::PopStyleVar();
		if (beginResult)
		{
			ImRect cliprect = ImGui::GetCurrentWindow()->ClipRect;
			cliprect.Min.x -= ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Min.y -= ImGui::GetStyle().WindowPadding.y * 0.5f;
			cliprect.Max.x += ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Max.y += ImGui::GetStyle().WindowPadding.y * 0.5f;
			ImGui::PopClipRect();
			ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);

			if (ImGui::IsWindowAppearing())
			{
				game::World* world = game::World::GetInstance();
				world->AddScene(_scene);
			}
			
			DrawContent();

			if (_wasFocus == false && ImGui::IsWindowFocused())
			{
				Editor::GetInstance()->SetCurrentViewport(this);
				_wasFocus = true;
			}
		}
		else
		{
			game::World* world = game::World::GetInstance();
			world->RemoveScene(_scene);
			if (_wasFocus == true && Editor::GetInstance()->GetEntitySelection() != nullptr)
			{
				Editor::GetInstance()->SetEntitySelection(nullptr);
			}
			if (Editor::GetInstance()->GetCurrentViewport() == this)
			{
				Editor::GetInstance()->SetCurrentViewport(nullptr);
			}
			_wasFocus = false;
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
				uint32_t flags = physics::Physics::GetInstance()->GetDebugDrawer()->GetFlags();

				for (const physics::DebugDrawer::Flag& flag : physics::Physics::GetInstance()->GetDebugDrawer()->GetAvailableFlags())
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
						physics::Physics::GetInstance()->GetDebugDrawer()->SetFlags(flags);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		/*
		game::World* world = game::World::GetInstance();
		if (_selectedTab != nullptr)
		{
			world->RemoveScene(_selectedTab->_scene);
		}
		world->AddScene(tab->_scene);
		*/

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
				_size -= ImGui::GetIO().MouseWheel * 0.016f * std::abs(_size);
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

				if (mousePos.x >= 0 && mousePos.x < _pickingRenderTarget->GetWidth() && mousePos.y >= 0 && mousePos.y < _pickingRenderTarget->GetHeight())
				{
					ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
					Vector2 mousePosition(mousePos.x, mousePos.y);
					Color pickingColor = _pickingRenderTarget->GetColorTexture()->ReadPixel(mousePosition);
					uint32_t pickingId = renderer::PickingManager::ConvertColorToId(pickingColor);
					if (pickingId == 0)
					{
						Editor::GetInstance()->SetEntitySelection(nullptr);
					}
					else
					{
						game::World* world = game::World::GetInstance();
						std::shared_ptr<game::Entity> pickedEntity = world->FindEntity((game::Entity::Id)pickingId).lock();
						if (pickedEntity != nullptr)
						{
							Editor::GetInstance()->SetEntitySelection(pickedEntity);
						}
					}					
				}
			}
		}

		uint32_t windowWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		uint32_t windowHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		windowWidth = std::clamp(windowWidth, 2u, 16u * 1024u);
		windowHeight = std::clamp(windowHeight, 2u, 16u * 1024u);

		if (_renderTarget->GetWidth() != windowWidth ||
			_renderTarget->GetHeight() != windowHeight)
		{
			renderer::Texture::CreateInfo createInfo;

			createInfo._allowReadWrite = false;
			_renderTarget->Init(windowWidth, windowHeight, createInfo); // todo error
			_renderTarget->PrepareForRead(); // todo automate ?

			createInfo._allowReadWrite = true;
			_pickingRenderTarget->Init(windowWidth, windowHeight, createInfo); // todo error
			_pickingRenderTarget->PrepareForRead(); // todo automate ?
		}

		if (_renderTarget->IsValid() == true)
		{
			ImVec2 origin = ImGui::GetCursorScreenPos();

			_renderQueue.Prepare(_renderTarget, _pickingRenderTarget);

			if (Editor::GetInstance()->IsPlaying() == false)
			{
				Rect viewport;
				viewport._position.SetX(0);
				viewport._position.SetY(0);
				viewport._size.SetX((float)windowWidth);
				viewport._size.SetY((float)windowHeight);

				float aspect = (float)windowWidth / (float)windowHeight;

				_projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
				_view = Matrix4::Translation(_cameraPosition);

				_renderQueue.PushRenderCommand(new renderer::RenderCommandSetCameraSettings(_projection, _view, viewport));

				game::World* world = game::World::GetInstance();
				world->Draw(&_renderQueue);

				if (_physicsDebugDrawer != nullptr)
				{
					_physicsDebugDrawer->PushToRenderQueue(_renderQueue);
				}

				Editor* editor = Editor::GetInstance();
				std::shared_ptr<game::Entity> sceneSelection = editor->GetEntitySelection();
				if (sceneSelection != nullptr)
				{
					for (std::weak_ptr<game::Component> component : sceneSelection->GetComponents())
					{
						std::shared_ptr<game::Component> componentLock = component.lock();
						if (componentLock != nullptr)
						{
							ReflectionTraitComponentCustomEditor* customEditorTrait = componentLock->GetReflectionDescriptorV()->FindTrait<ReflectionTraitComponentCustomEditor>();
							if (customEditorTrait != nullptr)
							{
								ComponentCustomEditor* customEditor = customEditorTrait->GetCustomEditor();
								if (customEditor != nullptr)
								{
									if (customEditor->OnDrawGizmo(componentLock, *this))
									{
										MarkCurrentSceneAsDirty();
									}
								}
							}
						}
					}
				}
			}
			else
			{
				game::World* world = game::World::GetInstance();
				world->Draw(&_renderQueue);
			}

			_renderQueue.Execute();

			if (_debugPicker)
			{
				ImGui::Image(_pickingRenderTarget->GetColorTexture(), ImVec2((float)windowWidth, (float)windowHeight));
			}
			else
			{
				ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)windowWidth, (float)windowHeight));
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
								Document document;
								DocumentReaderJson documentReader;
								if (documentReader.Read(document, asset->GetPath()) == false)
								{
									return; // todo message + bool
								}

								game::Prefab* prefab = new game::Prefab(asset->GetUid());
								if (Serializer::Deserialize(prefab, document.GetRootNode()) == false)
								{
									return; // todo message + bool
								}

								_scene->Instantiate(*prefab);

								MarkCurrentSceneAsDirty();
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	}

	/// @brief 
	void ViewportWindow::MarkCurrentSceneAsDirty()
	{
		if (_asset != nullptr)
		{
			_asset->SetDirty();
			_assetWasDirty = true;

			std::string title = std::format("{} " ICON_MDI_STAR_FOUR_POINTS_SMALL, _asset->GetName());
			SetTitle(title);
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
				_physicsDebugDrawer = new PhysicsDebugDrawer();
			}
		}
		else
		{
			if (_physicsDebugDrawer != nullptr)
			{
				delete _physicsDebugDrawer;
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
	std::shared_ptr<Asset> ViewportWindow::GetAsset() const
	{
		return _asset;
	}

	/// @brief 
	/// @return 
	renderer::RenderQueue* ViewportWindow::GetRenderQueue()
	{
		return &_renderQueue;
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
}
