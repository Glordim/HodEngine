#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/ImGuizmo.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp>
#include <HodEngine/Core/Rect.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>

#include <cmath>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(ViewportWindow, "Viewport", true)

	/// @brief 
	ViewportWindow::ViewportWindow()
	{
		_renderTarget = renderer::Renderer::GetInstance()->CreateRenderTarget();
	}

	/// @brief 
	ViewportWindow::~ViewportWindow()
	{
		delete _renderTarget;
	}

	/// @brief 
	void ViewportWindow::Draw()
	{
		float windowWidth = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		float windowHeight = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		windowWidth = std::fmax(2, windowWidth);
		windowHeight = std::fmax(2, windowHeight);

		if (_renderTarget->GetWidth() != windowWidth ||
			_renderTarget->GetHeight() != windowHeight)
		{
			_renderTarget->Init(windowWidth, windowHeight); // todo error
			_renderTarget->PrepareForRead(); // todo automate ?
		}

		// todo check if visible (docking tab) ?

		if (_renderTarget->IsValid() == true)
		{
			renderer::RenderQueue* renderQueue = renderer::Renderer::GetInstance()->GetRenderQueue();

			Rect viewport;
			viewport._size.SetX(windowWidth);
			viewport._size.SetY(windowHeight);
			viewport._position.SetX(0);
			viewport._position.SetY(0);

			float aspect = viewport._size.GetX() / viewport._size.GetY();

			//_view = Matrix4::Identity; //glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix());
			if (ImGui::GetIO().MouseWheel != 0.0f)
			{
				_size -= ImGui::GetIO().MouseWheel * 0.016f * std::abs(_size);
				//_view.Translation(Vector4(0.0f, 0.0f, 1.0f, 0.0f) * 0.016f * ImGui::GetIO().MouseWheel);
			}

			if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Middle] == true && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
			{
				_view *= Matrix4::Translation(Vector2(ImGui::GetIO().MouseDelta.x * 0.01f, ImGui::GetIO().MouseDelta.y * 0.01f));
			}

			Matrix4 projection = Matrix4::OrthogonalProjection(-_size * aspect, _size * aspect, -_size, _size, -1024, 1024);

			renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, _view, viewport));

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

			_renderTarget->PrepareForWrite(); // todo automate ?

			renderQueue->Execute(_renderTarget);

			_renderTarget->PrepareForRead(); // todo automate ?

			ImGui::Image(_renderTarget->GetColorTexture(), ImVec2(windowWidth, windowHeight));

			Editor* editor = Editor::GetInstance();
			std::shared_ptr<game::Entity> sceneSelection = editor->GetEntitySelection().lock();
			if (sceneSelection != nullptr)
			{
				std::shared_ptr<game::Node2dComponent> node2D = sceneSelection->GetComponent<game::Node2dComponent>().lock();
				Matrix4 localMatrix = node2D->GetLocalMatrix();
				ImGuizmo::SetOrthographic(true);
				ImGuiIO& io = ImGui::GetIO();
   				ImGuizmo::SetRect(ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMin().y, windowWidth, windowHeight);
				ImGuizmo::Manipulate((float*)&_view, (float*)&projection, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, (float*)&localMatrix);
			}
		}
	}
}
