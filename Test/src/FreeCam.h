#pragma once

#include "HodEngine/Game/src/Actor.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

typedef struct CTwBar TwBar;

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

        void Update(float dt) override;

    private:

		void selectObject(int mouseX, int mouseY);

        void rotateView(int x, int y);

        GAME::SceneComponent* sceneComponent;
        GAME::CameraComponent* cameraComponent;

        glm::vec3 movement;
        glm::vec3 view;

        glm::quat tmpQuat;

        float mouseX;
        float mouseY;

		bool move = false;
		bool released = true;

        float speed = 5.0f;
    };
}
