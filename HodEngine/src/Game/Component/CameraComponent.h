#pragma once

#include <vector>

#include "Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include "../Scene.h"

namespace HOD
{
    class MaterialInstance;
    class Texture;

    namespace GAME
    {
        class PointLightComponent;

        class CameraComponent : public Component
        {
        public:
                                CameraComponent(Actor* actor);
                                CameraComponent(const CameraComponent&) = delete;
                                ~CameraComponent() override = default;

            void                setupTweakBar(TwBar* tweakBar) override;

            void                render(Scene& scene);

            const glm::mat4&    getProjectionMatrix();

            void                SetHdriMaterial(MaterialInstance* hdriMat, Texture* hdriTexture);

        private:
            float               _fov;
            float               _aspect;
            float               _fNear;
            float               _fFar;
            bool                _perspective;
            bool                _dirtyFlag;

            MaterialInstance*   _hdriMat;
            Texture*            _hdriTexture;

            glm::mat4           _projectionMatrix;
        };
    }
}
