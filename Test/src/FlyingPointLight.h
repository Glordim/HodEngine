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
    }

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
    class FlyingPointLight : public GAME::Actor
    {
    public:
        FlyingPointLight() = delete;
        FlyingPointLight(const std::string& name, GAME::Scene* scene);
        FlyingPointLight(const FlyingPointLight& copy) = delete;
        virtual ~FlyingPointLight();

        virtual void Start() override;
        virtual void Update(float dt) override;

    private:
        GAME::SceneComponent* sceneComponent;
    };
}
