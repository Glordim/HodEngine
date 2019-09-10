#ifndef __FREECAM_HPP__
#define __FREECAM_HPP__

#include "../Actor.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/vec3.hpp"

class SceneComponent;
class CameraComponent;

typedef struct CTwBar TwBar;

class FreeCam : public Actor
{
public:
    FreeCam() = delete;
    FreeCam(const std::string& name, Scene* scene);
    FreeCam(const FreeCam& copy) = delete;
    virtual ~FreeCam();

    virtual void setupInputListener(InputListener* inputListener) override;

    virtual void update(float dt) override;

private:

    void selectObject();

    void allowRotate();
    void disallowRotate();
    void rotateView(int x, int y);

    void moveForward(float axisValue);
    void moveRight(float axisValue);
    void moveUp(float axisValue);

    SceneComponent* sceneComponent;
    CameraComponent* cameraComponent;

    glm::vec3 movement;
    glm::vec3 view;

    bool allowRotation;

    glm::quat tmpQuat;

    float mouseX;
    float mouseY;

    TwBar* myBar;
};

#endif
