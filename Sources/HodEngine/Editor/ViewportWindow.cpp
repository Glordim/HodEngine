#include "HodEngine/Editor/ViewportWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Components/RendererComponent.hpp"
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp>
#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

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

			Matrix4 projection = Matrix4::OrthogonalProjection(-1 * aspect, 1 * aspect, -1, 1, -1024.0f, 1024.0f);
			Matrix4 view = Matrix4::Identity; //glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix());

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

			_renderTarget->PrepareForWrite(); // todo automate ?

			renderQueue->Execute(_renderTarget);

			_renderTarget->PrepareForRead(); // todo automate ?

			ImGui::Image(_renderTarget->GetColorTexture(), ImVec2(windowWidth, windowHeight));
		}
	}
}
