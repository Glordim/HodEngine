#include "CameraComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/gtc/matrix_transform.hpp"

#include "SceneComponent.h"

//#include "AntTweakBar.h"

#include "../../Renderer/Renderer.h"
#include "../../Renderer/RenderQueue.h"
#include "../../RenderQueueHelper.h"

namespace HOD
{
    namespace GAME
    {
        CameraComponent::CameraComponent(Actor* actor)
            : Component(actor)
            , _fov(60.0f)
            , _aspect(1920.0f / 1080.0f)
            , _fNear(0.01f)
            , _fFar(1000.0f)
            , _perspective(true)
            , _dirtyFlag(true)
            , _hdriMat(nullptr)
            , _hdriTexture(nullptr)
        {

        }

        void CameraComponent::DrawImGui()
        {
            //TwAddSeparator(tweakBar, "Camera", "");
            //TwAddVarRW(tweakBar, "fov", TW_TYPE_FLOAT, &this->fov, "");
            //TwAddVarRW(tweakBar, "near", TW_TYPE_FLOAT, &this->fNear, "");
            //TwAddVarRW(tweakBar, "far", TW_TYPE_FLOAT, &this->fFar, "");
        }

        const char* CameraComponent::GetName()
        {
            return "Camera";
        }

        void CameraComponent::SetHdriMaterial(MaterialInstance* hdriMat, Texture* hdriTexture)
        {
            _hdriMat = hdriMat;
            _hdriTexture = hdriTexture;
        }

        const glm::mat4& CameraComponent::getProjectionMatrix()
        {
            if (_dirtyFlag == true)
            {
                if (_perspective == true)
                    _projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _fNear, _fFar);
                else
                    _projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, _fNear, _fFar);

                _dirtyFlag = false;
            }

            return _projectionMatrix;
        }

        void CameraComponent::render(Scene& scene)
        {
            RenderQueue renderQueue;

            renderQueue.SetCameraPos(GetActor()->getComponent<SceneComponent>()->getPosition());
            renderQueue.SetViewMatrix(glm::inverse(GetActor()->getComponent<SceneComponent>()->getModelMatrix()));
            renderQueue.SetProjMatrix(this->getProjectionMatrix());

            renderQueue.SetClearFlag(RenderQueue::ClearFlag::COLOR | RenderQueue::ClearFlag::DEPTH);

            renderQueue.SetHdriMaterial(_hdriMat);
            renderQueue.SetHdriTexture(_hdriTexture);

            RenderQueueHelper::AddSceneComponent(renderQueue, scene.getRoot(), true);
            RenderQueueHelper::AddScenePhysicsDebug(renderQueue, &scene);

            Renderer* renderer = Renderer::GetInstance();
            renderer->SubmitRenderQueue(renderQueue);
        }
    }
}
