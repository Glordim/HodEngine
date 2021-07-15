#include "FreeCam.h"

#include "HodEngine/Game/src/Game.h"
#include "HodEngine/Game/src/Component/SceneComponent.h"
#include "HodEngine/Game/src/Component/CameraComponent.h"

#include "HodEngine/Game/src/InputListener.h"

#include <HodEngine/Physics/src/Scene.h>

#include <SDL2/SDL.h>

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
		_sceneComponent = AddComponent<GAME::SceneComponent>();
		_cameraComponent = AddComponent<GAME::CameraComponent>();

		_view = glm::vec3(0.0f, 0.0f, 0.0f);
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
	void FreeCam::SelectObject(int mouseX, int mouseY)
	{
		if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
			return;

		float mouseZ = 1.0f;

		glm::mat4 screenToWorldMat = _cameraComponent->GetProjectionMatrix() * glm::inverse(_sceneComponent->GetModelMatrix());
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
		/*
		glm::vec3 finalPos = _sceneComponent->GetPosition() + glm::vec3(dir * 100.0f);

		PHYSICS::RaycastResult result;

		if (_scene->Raycast(_sceneComponent->GetPosition(), dir, 100.0f, result, true, CORE::Color(1.0f, 1.0f, 0.0f, 1.0f), 5.0f) == true)
		{
			PHYSICS::Actor* physicActor = result._actorCollided;

			GAME::Actor* actor = _scene->ConvertPxActor(physicActor);
			if (actor != nullptr)
			{
				GAME::Game::GetInstance()->DebugActor(actor);
			}
		}
		*/
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void FreeCam::RotateView(int x, int y)
	{
		_mouseX = (float)x;
		_mouseY = (float)y;

		APPLICATION::Application* app = APPLICATION::Application::GetInstance();

		int width = app->GetWidth();
		int height = app->GetHeight();

		float angleX = ((float)x - ((float)width * 0.5f)) / ((float)width * 0.5f);
		float angleY = ((float)y - ((float)height * 0.5f)) / ((float)height * 0.5f);

		_view.x += angleX;
		_view.y += angleY;

		_sceneComponent->SetRotation(0.0f);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void FreeCam::Update(float dt)
	{
		if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_E] > 0.0f)
		{
			_speed += 10.0f * dt;
		}
		else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_Q] > 0.0f)
		{
			_speed -= 10.0f * dt;
			if (_speed < 0.0f)
			{
				_speed = 0.1f;
			}
		}

		int mouseX = 0;
		int mouseY = 0;

		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		if (mouseState & SDL_BUTTON(3))
		{
			APPLICATION::Application* app = APPLICATION::Application::GetInstance();

			int width = app->GetWidth();
			int height = app->GetHeight();

			if (_move == true)
			{
				RotateView(mouseX, mouseY);

				if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_W] > 0.0f)
				{
					_movement.z = 1.0f;
				}
				else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_S] > 0.0f)
				{
					_movement.z = -1.0f;
				}
				else
				{
					_movement.z = 0.0f;
				}

				if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] > 0.0f)
				{
					_movement.x = -1.0f;
				}
				else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_D] > 0.0f)
				{
					_movement.x = 1.0f;
				}
				else
				{
					_movement.x = 0.0f;
				}

				if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE] > 0.0f)
				{
					_movement.y = 1.0f;
				}
				else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_C] > 0.0f)
				{
					_movement.y = -1.0f;
				}
				else
				{
					_movement.y = 0.0f;
				}

				//_sceneComponent->SetPosition(_sceneComponent->GetPosition() + (_sceneComponent->GetRotation() * _movement * dt * _speed));
			}
			else
			{
				if (mouseX == (width / 2) && mouseY == (height / 2))
				{
					_move = true;
				}
			}

			HOD::APPLICATION::Application::GetInstance()->SetCursorPosition(width / 2, height / 2);
		}
		else
		{
			_move = false;
		}

		if (mouseState & SDL_BUTTON(1))
		{
			if (_released == true)
			{
				SelectObject(mouseX, mouseY);
			}
			_released = false;
		}
		else
		{
			_released = true;
		}

		if (mouseState & SDL_BUTTON(2))
		{
			GAME::Game::GetInstance()->DebugActor(this);
		}
	}
}
