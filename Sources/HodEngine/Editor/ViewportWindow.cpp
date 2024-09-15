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

#include <cmath>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(ViewportWindow, "Viewport", false)

	/// @brief 
	ViewportWindow::ViewportWindow()
	{
		SetFlags(ImGuiWindowFlags_MenuBar);

		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_pickingRenderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();

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
			_prefab = new game::Prefab();
			if (Serializer::Deserialize(_prefab, document.GetRootNode()) == false)
			{
				return; // todo message + bool
			}
			std::shared_ptr<game::Entity> prefabRootEntity = _scene->Instantiate(*_prefab);
			if (prefabRootEntity != nullptr) // TODO a Prefab should not be empty
			{
				prefabRootEntity->SetPrefab(nullptr); // Unpack prefab for serialization, otherwise that will be serialize as PrefabInstance
			}
			asset->SetInstanceToSave(_scene, _scene->GetReflectionDescriptorV());
		}

		game::World* world = game::World::GetInstance();
		world->AddScene(_scene);
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

	bool ViewportWindow::Draw()
	{
		bool open = true;

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
		}
		else
		{
			game::World* world = game::World::GetInstance();
			world->RemoveScene(_scene);
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
				_gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_R))
			{
				_gizmoOperation = ImGuizmo::OPERATION::ROTATE_Z;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_S))
			{
				_gizmoOperation = ImGuizmo::OPERATION::SCALE;
			}

			if (ImGui::GetIO().MouseWheel != 0.0f)
			{
				_size -= ImGui::GetIO().MouseWheel * 0.016f * std::abs(_size);
			}

			if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] == true && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
			{
				Vector2 movement(ImGui::GetIO().MouseDelta.x * 0.01f, -ImGui::GetIO().MouseDelta.y * 0.01f);
				_cameraPosition.SetX(_cameraPosition.GetX() + movement.GetX());
				_cameraPosition.SetY(_cameraPosition.GetY() + movement.GetY());
			}

			if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left] == true && ImGui::IsWindowHovered() && ImGuizmo::IsOver() == false && ImGuizmo::IsUsing() == false)
			{
				ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();

				if (mousePos.x >= 0 && mousePos.x < _pickingRenderTarget->GetWidth() && mousePos.y >= 0 && mousePos.y < _pickingRenderTarget->GetHeight())
				{
					_pickingRequest = true;
					_pickingPosition = Vector2(mousePos.x, mousePos.y);
				}
			}
		}

		uint32_t windowWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		uint32_t windowHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		windowWidth = std::clamp(windowWidth, 2u, 16u * 1024u);
		windowHeight = std::clamp(windowHeight, 2u, 16u * 1024u);

		// todo check if visible (docking tab) ?

		if (_pickingRequest == true)
		{
			if (_pickingRenderTarget->GetWidth() != windowWidth ||
				_pickingRenderTarget->GetHeight() != windowHeight)
			{
				renderer::Texture::CreateInfo createInfo;
				createInfo._allowReadWrite = true;
				_pickingRenderTarget->Init(windowWidth, windowHeight, createInfo); // todo error
				_pickingRenderTarget->PrepareForRead(); // todo automate ?
			}

			if (_pickingRenderTarget->IsValid() == true)
			{
				renderer::RenderQueue* renderQueue = renderer::RenderQueue::GetInstance();

				Rect viewport;
				viewport._position.SetX(0);
				viewport._position.SetY((float)windowHeight);
				viewport._size.SetX((float)windowWidth);
				viewport._size.SetY(-(float)windowHeight);
				// Vulkan specific Y inversion TODO move it in Vulkan part and probably readapt ImguiRenderer to aply this inversion because it need 2D bottom y axis

				float aspect = (float)windowWidth / (float)windowHeight;

				Matrix4 projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
				Matrix4 view = Matrix4::Translation(_cameraPosition);

				renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

				std::map<uint32_t, std::shared_ptr<game::RendererComponent>> colorIdToRendererComponentMap;

				game::World* world = game::World::GetInstance();
				world->DrawPicking(renderQueue, colorIdToRendererComponentMap);

				_pickingRenderTarget->PrepareForWrite(); // todo automate ?

				renderQueue->Execute(_pickingRenderTarget);

				_pickingRenderTarget->PrepareForRead(); // todo automate ?

				Color pixelColor = _pickingRenderTarget->GetColorTexture()->ReadPixel(_pickingPosition);

				union IdToColorConverter
				{
					uint32_t uint32;
					uint8_t uint8[4];
				};
				IdToColorConverter idToColorConverter;

				idToColorConverter.uint8[0] = static_cast<uint8_t>(pixelColor.r * 255.0f);
				idToColorConverter.uint8[1] = static_cast<uint8_t>(pixelColor.g * 255.0f);
				idToColorConverter.uint8[2] = static_cast<uint8_t>(pixelColor.b * 255.0f);
				idToColorConverter.uint8[3] = 0;

				auto it = colorIdToRendererComponentMap.find(idToColorConverter.uint32);
				if (it != colorIdToRendererComponentMap.end())
				{
					Editor::GetInstance()->SetEntitySelection(it->second->GetEntity());
				}
				else
				{
					Editor::GetInstance()->SetEntitySelection(std::weak_ptr<game::Entity>());
				}

				//ImVec2 origin = ImGui::GetCursorScreenPos();
				//ImGui::Image(_pickingRenderTarget->GetColorTexture(), ImVec2(windowWidth, windowHeight));
				//return;
				_pickingRequest = false;
			}
		}

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
			renderer::RenderQueue* renderQueue = renderer::RenderQueue::GetInstance();

			Rect viewport;
			viewport._position.SetX(0);
			viewport._position.SetY(0);
			viewport._size.SetX((float)windowWidth);
			viewport._size.SetY((float)windowHeight);

			float aspect = (float)windowWidth / (float)windowHeight;

			Matrix4 projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
			Matrix4 view = Matrix4::Translation(_cameraPosition);

			renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

			game::World* world = game::World::GetInstance();
			world->Draw(renderQueue);

			if (_physicsDebugDrawer != nullptr)
			{
				_physicsDebugDrawer->PushToRenderQueue(*renderQueue);
			}

			Editor* editor = Editor::GetInstance();
			std::shared_ptr<game::Entity> sceneSelection = editor->GetEntitySelection().lock();
			if (sceneSelection != nullptr)
			{
				ImGuizmo::SetOrthographic(true);
				ImGuizmo::SetRect(origin.x, origin.y, (float)windowWidth, (float)windowHeight);
				ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

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
								if (customEditor->OnDrawGizmo(componentLock, projection, view, _gizmoOperation, *renderQueue))
								{
									Editor::GetInstance()->MarkCurrentSceneAsDirty();
								}
							}
						}
					}
				}
			}

			_renderTarget->PrepareForWrite(); // todo automate ?
			_pickingRenderTarget->PrepareForWrite(); // todo automate ?

			renderQueue->Execute(_renderTarget, _pickingRenderTarget);

			_renderTarget->PrepareForRead(); // todo automate ?
			_pickingRenderTarget->PrepareForRead(); // todo automate ?

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

								Editor::GetInstance()->MarkCurrentSceneAsDirty();
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
}
