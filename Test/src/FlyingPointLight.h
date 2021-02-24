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
								FlyingPointLight(const std::string& name, GAME::Scene* scene);
								FlyingPointLight(const FlyingPointLight&) = delete;
								FlyingPointLight(FlyingPointLight&&) = delete;
								~FlyingPointLight() override;

		void					operator=(const FlyingPointLight&) = delete;
		void					operator=(FlyingPointLight&&) = delete;

	public:

		void					Start() override;
		void					Update(float dt) override;

	private:

		GAME::SceneComponent*	_sceneComponent;
	};
}
