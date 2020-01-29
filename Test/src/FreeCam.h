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

        void setupInputListener(InputListener* inputListener) override;

        void update(float dt) override;

    private:

        void selectSelfObject();
        void selectObject();

        void allowRotate();
        void disallowRotate();
        void rotateView(int x, int y);

        void moveForward(float axisValue);
        void moveRight(float axisValue);
        void moveUp(float axisValue);

        GAME::SceneComponent* sceneComponent;
        GAME::CameraComponent* cameraComponent;

        glm::vec3 movement;
        glm::vec3 view;

        bool allowRotation;

        glm::quat tmpQuat;

        float mouseX;
        float mouseY;

        TwBar* myBar;
    };
}
