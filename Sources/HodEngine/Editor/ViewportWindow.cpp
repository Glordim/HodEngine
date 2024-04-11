#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/ImGuiManager.hpp>

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

				_pickingRequest = true;
				_pickingPosition = Vector2(mousePos.x, mousePos.y);
			}
		}

		float windowWidth = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		float windowHeight = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		windowWidth = std::fmax(2, windowWidth);
		windowHeight = std::fmax(2, windowHeight);

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
				viewport._position.SetY(windowHeight);
				viewport._size.SetX(windowWidth);
				viewport._size.SetY(-windowHeight);
				// Vulkan specific Y inversion TODO move it in Vulkan part and probably readapt ImguiRenderer to aply this inversion because it need 2D bottom y axis

				float aspect = windowWidth / windowHeight;

				Matrix4 projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
				Matrix4 view = Matrix4::Translation(_cameraPosition);

				renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

				uint32_t id = 0;
				union IdToColorConverter
				{
					uint32_t uint32;
					uint8_t uint8[4];
				};
				IdToColorConverter idToColorConverter;

				std::map<uint32_t, std::shared_ptr<game::RendererComponent>> _colorIdToRendererComponentMap;

				game::World* world = game::World::GetInstance();
				for (const auto& pair : world->GetEntities())
				{
					std::shared_ptr<game::Entity> entity = pair.second;
					std::shared_ptr<game::RendererComponent> rendererComponent = entity->GetComponent<game::RendererComponent>().lock();
					if (rendererComponent != nullptr)
					{
						idToColorConverter.uint32 = id;

						Color colorId;
						colorId.r = static_cast<float>(idToColorConverter.uint8[0]) / 255.0f;
						colorId.g = static_cast<float>(idToColorConverter.uint8[1]) / 255.0f;
						colorId.b = static_cast<float>(idToColorConverter.uint8[2]) / 255.0f;
						colorId.a = 1.0f;

						rendererComponent->PushPickingToRenderQueue(*renderQueue, colorId);

						_colorIdToRendererComponentMap.emplace(idToColorConverter.uint32, rendererComponent);

						id += 50;
					}
				}
				world->Draw(renderQueue);

				_pickingRenderTarget->PrepareForWrite(); // todo automate ?

				renderQueue->Execute(_pickingRenderTarget);

				_pickingRenderTarget->PrepareForRead(); // todo automate ?

				Color pixelColor = _pickingRenderTarget->GetColorTexture()->ReadPixel(_pickingPosition);

				idToColorConverter.uint8[0] = pixelColor.r * 255.0f;
				idToColorConverter.uint8[1] = pixelColor.g * 255.0f;
				idToColorConverter.uint8[2] = pixelColor.b * 255.0f;
				idToColorConverter.uint8[3] = 0;

				auto it = _colorIdToRendererComponentMap.find(idToColorConverter.uint32);
				if (it != _colorIdToRendererComponentMap.end())
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
			viewport._position.SetY(windowHeight);
			viewport._size.SetX(windowWidth);
			viewport._size.SetY(-windowHeight);
			// Vulkan specific Y inversion TODO move it in Vulkan part and probably readapt ImguiRenderer to aply this inversion because it need 2D bottom y axis

			float aspect = windowWidth / windowHeight;

			Matrix4 projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);
			Matrix4 view = Matrix4::Translation(_cameraPosition);

			renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

			game::World* world = game::World::GetInstance();
			for (const auto& pair : world->GetEntities())
			{
				std::shared_ptr<game::Entity> entity = pair.second;
				std::shared_ptr<game::RendererComponent> rendererComponent = entity->GetComponent<game::RendererComponent>().lock();
				if (rendererComponent != nullptr)
				{
					rendererComponent->PushToRenderQueue(*renderQueue);
				}
			}
			world->Draw(renderQueue);

			_renderTarget->PrepareForWrite(); // todo automate ?

			renderQueue->Execute(_renderTarget);

			_renderTarget->PrepareForRead(); // todo automate ?

			ImVec2 origin = ImGui::GetCursorScreenPos();
			ImGui::Image(_renderTarget->GetColorTexture(), ImVec2(windowWidth, windowHeight));

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
					position[0] = _cameraPosition.GetX();
					position[1] = _cameraPosition.GetY();
					rotation[2] = 0.0f;
					scale[0] = 1.0f;
					scale[1] = 1.0f;
					ImGuizmo::RecomposeMatrixFromComponents(position, rotation, scale, viewMatrix);

					ImGuizmo::SetOrthographic(true);
					ImGuiIO& io = ImGui::GetIO();
					ImGuizmo::SetRect(origin.x, origin.y, windowWidth, windowHeight);
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
}
