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

#include <cmath>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(ViewportWindow, "Viewport", true)

	/// @brief 
	ViewportWindow::ViewportWindow()
	{
		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
		_pickingRenderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();

		game::World::GetInstance()->EnablePhysicsDebugDrawer(true);
	}

	/// @brief 
	ViewportWindow::~ViewportWindow()
	{
		delete _renderTarget;
		delete _pickingRenderTarget;
	}

	/// @brief 
	void ViewportWindow::Draw()
	{
		if (ImGui::BeginTabBar("##TabBar"))
		{
			auto it = _tabs.begin();
			auto itEnd = _tabs.end();
			while (it != itEnd)
			{
				Tab* tab = *it;
				bool open = true;

				ImGuiTabItemFlags flags = 0;
				if (_tabToSelect == tab)
				{
					_tabToSelect = nullptr;
					flags = ImGuiTabItemFlags_SetSelected;
				}

				char tabName[256];
				std::strcpy(tabName, tab->_asset->GetName().c_str());
				if (tab->_asset->IsDirty())
				{
					std::strcat(tabName, ICON_MDI_RHOMBUS_MEDIUM);
				}
				ImGui::PushID(tab);
				bool selected = ImGui::BeginTabItem(tabName, &open, flags);
				ImGui::PopID();
				if (selected == true)
				{
					if (tab != _selectedTab)
					{
						game::World* world = game::World::GetInstance();
						if (_selectedTab != nullptr)
						{
							world->RemoveScene(_selectedTab->_scene);
						}
						world->AddScene(tab->_scene);

						_selectedTab = tab;
					}

					DrawTab(tab);

					ImGui::EndTabItem();
				}

				if (open == false)
				{
					it = CloseTab(it);
					itEnd = _tabs.end();
					continue;
				}
				
				++it;
			}

			ImGui::EndTabBar();
		}
	}

	/// @brief 
	/// @param tab 
	void ViewportWindow::DrawTab(Tab* tab)
	{
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
				tab->_size -= ImGui::GetIO().MouseWheel * 0.016f * std::abs(tab->_size);
			}

			if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] == true && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
			{
				Vector2 movement(ImGui::GetIO().MouseDelta.x * 0.01f, -ImGui::GetIO().MouseDelta.y * 0.01f);
				tab->_cameraPosition.SetX(tab->_cameraPosition.GetX() + movement.GetX());
				tab->_cameraPosition.SetY(tab->_cameraPosition.GetY() + movement.GetY());
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

		uint32_t windowWidth = (uint32_t)ImGui::GetContentRegionAvail().x;// ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		uint32_t windowHeight = (uint32_t)ImGui::GetContentRegionAvail().y; //ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		windowWidth = std::max(2u, windowWidth);
		windowHeight = std::max(2u, windowHeight);

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
				renderer::RenderQueue* renderQueue = renderer::Renderer::GetInstance()->GetRenderQueue();

				Rect viewport;
				viewport._position.SetX(0);
				viewport._position.SetY((float)windowHeight);
				viewport._size.SetX((float)windowWidth);
				viewport._size.SetY(-(float)windowHeight);
				// Vulkan specific Y inversion TODO move it in Vulkan part and probably readapt ImguiRenderer to aply this inversion because it need 2D bottom y axis

				float aspect = (float)windowWidth / (float)windowHeight;

				Matrix4 projection = Matrix4::OrthogonalProjection(-tab->_size * aspect, tab->_size * aspect, -tab->_size, tab->_size, -1024, 1024);
				Matrix4 view = Matrix4::Translation(tab->_cameraPosition);

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
			_renderTarget->Init(windowWidth, windowHeight, renderer::Texture::CreateInfo()); // todo error
			_renderTarget->PrepareForRead(); // todo automate ?
		}

		if (_renderTarget->IsValid() == true)
		{
			renderer::RenderQueue* renderQueue = renderer::Renderer::GetInstance()->GetRenderQueue();

			Rect viewport;
			viewport._position.SetX(0);
			viewport._position.SetY((float)windowHeight);
			viewport._size.SetX((float)windowWidth);
			viewport._size.SetY(-(float)windowHeight);
			// Vulkan specific Y inversion TODO move it in Vulkan part and probably readapt ImguiRenderer to aply this inversion because it need 2D bottom y axis

			float aspect = (float)windowWidth / (float)windowHeight;

			Matrix4 projection = Matrix4::OrthogonalProjection(-tab->_size * aspect, tab->_size * aspect, -tab->_size, tab->_size, -1024, 1024);
			Matrix4 view = Matrix4::Translation(tab->_cameraPosition);

			renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

			game::World* world = game::World::GetInstance();
			world->Draw(renderQueue);

			_renderTarget->PrepareForWrite(); // todo automate ?

			renderQueue->Execute(_renderTarget);

			_renderTarget->PrepareForRead(); // todo automate ?

			ImVec2 origin = ImGui::GetCursorScreenPos();
			ImGui::Image(_renderTarget->GetColorTexture(), ImVec2((float)windowWidth, (float)windowHeight));
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

								tab->_scene->Instantiate(*prefab);

								Editor::GetInstance()->MarkCurrentSceneAsDirty();
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			Editor* editor = Editor::GetInstance();
			std::shared_ptr<game::Entity> sceneSelection = editor->GetEntitySelection().lock();
			if (sceneSelection != nullptr)
			{
				std::shared_ptr<game::Node2dComponent> node2D = sceneSelection->GetComponent<game::Node2dComponent>().lock();
				if (node2D != nullptr)
				{
					float matrix[16];
					float position[3];
					position[0] = node2D->GetPosition().GetX();
					position[1] = node2D->GetPosition().GetY();
					position[2] = 0.0f;
					float rotation[3];
					rotation[0] = 0.0f;
					rotation[1] = 0.0f;
					rotation[2] = node2D->GetRotation();
					float scale[3];
					scale[0] = node2D->GetScale().GetX();
					scale[1] = node2D->GetScale().GetY();
					scale[2] = 1.0f;
					ImGuizmo::RecomposeMatrixFromComponents(position, rotation, scale, matrix);

					float viewMatrix[16];
					position[0] = tab->_cameraPosition.GetX();
					position[1] = tab->_cameraPosition.GetY();
					rotation[2] = 0.0f;
					scale[0] = 1.0f;
					scale[1] = 1.0f;
					ImGuizmo::RecomposeMatrixFromComponents(position, rotation, scale, viewMatrix);

					ImGuizmo::SetOrthographic(true);
					ImGuizmo::SetRect(origin.x, origin.y, (float)windowWidth, (float)windowHeight);
					ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
					if (ImGuizmo::Manipulate(viewMatrix, (float*)&projection, _gizmoOperation, ImGuizmo::MODE::LOCAL, matrix))
					{
						ImGuizmo::DecomposeMatrixToComponents(matrix, position, rotation, scale);
						node2D->SetPosition(Vector2(position[0], position[1]));
						node2D->SetRotation(rotation[2]);
						node2D->SetScale(Vector2(scale[0], scale[1]));
						Editor::GetInstance()->MarkCurrentSceneAsDirty();
					}
				}
			}
		}
	}

	/// @brief 
	/// @param asset 
	void ViewportWindow::OpenTab(std::shared_ptr<Asset> asset)
	{
		for (Tab* tab : _tabs)
		{
			if (tab->_asset == asset)
			{
				_tabToSelect = tab;
				return;
			}
		}

		Tab* tab = new Tab(asset);

		_tabs.push_back(tab);
		_tabToSelect = tab;
	}

	/// @brief 
	/// @param asset 
	std::vector<ViewportWindow::Tab*>::iterator	ViewportWindow::CloseTab(std::vector<Tab*>::iterator it)
	{
		Tab* tab = *it;
		if (tab == _selectedTab)
		{
			game::World* world = game::World::GetInstance();
			world->RemoveScene(_selectedTab->_scene);
			_selectedTab = nullptr;
		}

		delete tab;
		return _tabs.erase(it);
	}

	/// @brief 
	void ViewportWindow::MarkCurrentSceneAsDirty()
	{
		if (_selectedTab != nullptr)
		{
			_selectedTab->_asset->SetDirty();
		}
	}

	/// @brief 
	/// @param asset 
	ViewportWindow::Tab::Tab(std::shared_ptr<Asset> asset)
	: _asset(asset)
	{
		Document document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, asset->GetPath()) == false)
		{
			return; // todo message + bool
		}

		_scene = new game::Scene();
		_scene->SetName(asset->GetName());

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
	}

	/// @brief 
	ViewportWindow::Tab::~Tab()
	{
		_asset->SetInstanceToSave(nullptr, nullptr);
		_asset->ResetDirty();
		delete _scene;
	}
}
