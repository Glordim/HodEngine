#include "HodEngine/Editor/ViewportWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Game/World.h"
#include "HodEngine/Game/Components/RendererComponent.h"
#include <HodEngine/Renderer/Renderer.h>
#include <HodEngine/Renderer/RHI/RenderTarget.h>
#include <HodEngine/Renderer/RHI/Texture.h>
#include <HodEngine/Renderer/RenderQueue.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.h>
#include "HodEngine/Core/Rect.h"
#include "HodEngine/Core/Math/Matrix4.h"

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
