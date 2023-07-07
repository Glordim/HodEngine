#include "HodEngine/Editor/ViewportWindow.h"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Game/World.h"
#include "HodEngine/Game/Components/RendererComponent.h"
#include <HodEngine/Renderer/Renderer.h>
#include <HodEngine/Renderer/RenderQueue.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.h>
#include "HodEngine/Core/Rect.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(ViewportWindow, "Viewport", true)

	/// @brief 
	void ViewportWindow::Draw()
	{
		// todo check if visible (docking tab) ?

		renderer::RenderQueue* renderQueue = renderer::Renderer::GetInstance()->GetRenderQueue();

		Rect viewport;
		viewport._size.x = ImGui::GetWindowWidth();
		viewport._size.y = ImGui::GetWindowHeight();
		viewport._position.x = ImGui::GetWindowPos().x;
		viewport._position.y = ImGui::GetWindowPos().y;

		float aspect = ImGui::GetWindowWidth() / ImGui::GetWindowHeight();
		glm::mat4 projection = glm::ortho(-aspect * 0.5f, aspect * 0.5f, -0.5f, 0.5f, 0.0f, 1000.0f);
		glm::mat4 view = glm::mat4(1.0f);//glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix());

		renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));

		game::World* world = game::World::GetInstance();
		for (const auto& pair : world->GetEntities())
		{
			std::shared_ptr<game::Entity> entity = pair.second;
			std::shared_ptr<game::RendererComponent> rendererComponent = entity->GetComponent<game::RendererComponent>().lock();
			if (rendererComponent != nullptr)
			{
				rendererComponent->Render();
			}
		}
	}
}
