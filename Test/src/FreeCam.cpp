#include "FreeCam.h"

#include "HodEngine/Game/src/Game.h"
#include "HodEngine/Game/src/Component/SceneComponent.h"
#include "HodEngine/Game/src/Component/CameraComponent.h"

#include "HodEngine/Game/src/InputListener.h"

#include <HodEngine/Physics/src/Scene.h>

#include <SDK/SDL/include/SDL.h>

#include "HodEngine/DebugLayer/src/DebugLayer.h"

#include <HodEngine/Application/src/Application.h>

#include "ImGui/src/ImGuizmo.h"

namespace HOD
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    FreeCam::FreeCam(const std::string& name, GAME::Scene* scene) : GAME::Actor(name, scene)
    {
        this->sceneComponent = this->addComponent<GAME::SceneComponent>();
        this->cameraComponent = this->addComponent<GAME::CameraComponent>();

        this->view = glm::vec3(0.0f, 0.0f, 0.0f);
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    FreeCam::~FreeCam()
    {
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    void FreeCam::selectObject(int mouseX, int mouseY)
    {
		if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
			return;

        float mouseZ = 1.0f;

        glm::mat4 screenToWorldMat = this->cameraComponent->getProjectionMatrix() * glm::inverse(this->sceneComponent->getModelMatrix());
        screenToWorldMat = glm::inverse(screenToWorldMat);

		APPLICATION::Application* app = APPLICATION::Application::GetInstance();

		int width = app->GetWidth();
		int height = app->GetHeight();

        glm::vec4 dir;
        dir.x = (mouseX - 0.0f) / (float)width * 2.0f - 1.0f;
        dir.y = (((float)height - mouseY - 0.0f)) / (float)height * 2.0f - 1.0f;
        dir.z = 2.0f * mouseZ - 1.0f;
        dir.w = 1.0f;

        dir = screenToWorldMat * dir;

        dir.w = 1.0f / dir.w;

        dir.x *= dir.w;
        dir.y *= dir.w;
        dir.z *= dir.w;

        dir = glm::normalize(dir);

        glm::vec3 finalPos = this->sceneComponent->getPosition() + glm::vec3(dir * 100.0f);

		PHYSICS::RaycastResult result;

        if (this->scene->raycast(this->sceneComponent->getPosition(), dir, 100.0f, result, true, CORE::Color(1.0f, 1.0f, 0.0f, 1.0f), 5.0f) == true)
        {
			PHYSICS::Actor* physicActor = result._actorCollided;

            GAME::Actor* actor = this->scene->convertPxActor(physicActor);
            if (actor != nullptr)
            {
				GAME::Game::GetInstance()->DebugActor(actor);
            }
        }
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    void FreeCam::rotateView(int x, int y)
    {
        this->mouseX = (float)x;
        this->mouseY = (float)y;

		APPLICATION::Application* app = APPLICATION::Application::GetInstance();

		int width = app->GetWidth();
		int height = app->GetHeight();

        float angleX = ((float)x - ((float)width * 0.5f)) / ((float)width * 0.5f);
        float angleY = ((float)y - ((float)height * 0.5f)) / ((float)height * 0.5f);

        this->view.x += angleX;
        this->view.y += angleY;

        this->sceneComponent->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        this->sceneComponent->rotate(view.x, glm::vec3(0.0f, 1.0f, 0.0f));
        this->sceneComponent->rotate(view.y, glm::vec3(1.0f, 0.0f, 0.0f));
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    void FreeCam::update(float dt)
    {
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_E] > 0.0f)
        {
            this->speed += 10.0f * dt;
        }
        else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_Q] > 0.0f)
        {
            this->speed -= 10.0f * dt;
            if (this->speed < 0.0f)
                this->speed = 0.1f;
        }

		int mouseX = 0;
		int mouseY = 0;

		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

        if (mouseState & SDL_BUTTON(3))
        {
			APPLICATION::Application* app = APPLICATION::Application::GetInstance();

			int width = app->GetWidth();
			int height = app->GetHeight();

			if (move == true)
			{
				rotateView(mouseX, mouseY);

				if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_W] > 0.0f)
				{
					this->movement.z = 1.0f;
				}
				else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_S] > 0.0f)
				{
					this->movement.z = -1.0f;
				}
				else
				{
					this->movement.z = 0.0f;
				}

				if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] > 0.0f)
				{
					this->movement.x = -1.0f;
				}
				else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_D] > 0.0f)
				{
					this->movement.x = 1.0f;
				}
				else
				{
					this->movement.x = 0.0f;
				}

				if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE] > 0.0f)
				{
					this->movement.y = 1.0f;
				}
				else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_C] > 0.0f)
				{
					this->movement.y = -1.0f;
				}
				else
				{
					this->movement.y = 0.0f;
				}

				this->sceneComponent->setPosition(sceneComponent->getPosition() + (sceneComponent->getRotation() * this->movement * dt * this->speed));
			}
			else
			{	
				if (mouseX == ((float)width * 0.5f) && mouseY == ((float)height * 0.5f))
				{
					move = true;
				}
			}

			HOD::APPLICATION::Application::GetInstance()->SetCursorPosition((float)width * 0.5f, (float)height * 0.5f);
        }
		else
		{
			move = false;
		}

        if (mouseState & SDL_BUTTON(1))
        {
			if (released == true)
			{
				selectObject(mouseX, mouseY);
			}
			released = false;
        }
		else
		{
			released = true;
		}

        if (mouseState & SDL_BUTTON(2))
        {
			GAME::Game::GetInstance()->DebugActor(this);
        }
    }
}
