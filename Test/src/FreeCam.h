#pragma once

#include "HodEngine/Game/src/Actor.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

namespace HOD
{
	namespace GAME
	{
		class SceneComponent;
		class CameraComponent;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class FreeCam : public GAME::Actor
	{
	public:

								FreeCam() = delete;
								FreeCam(const std::string& name, GAME::Scene* scene);
								FreeCam(const FreeCam&) = delete;
								~FreeCam() override;

		void					Update(float dt) override;

	private:

		void					SelectObject(int mouseX, int mouseY);
		void					RotateView(int x, int y);

	private:

		GAME::SceneComponent*	_sceneComponent;
		GAME::CameraComponent*	_cameraComponent;

		glm::vec3				_movement;
		glm::vec3				_view;

		glm::quat				_tmpQuat;

		float					_mouseX;
		float					_mouseY;

		bool					_move = false;
		bool					_released = true;

		float					_speed = 5.0f;
	};
}
