#pragma once

#include <PxPhysicsAPI.h>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/glm.hpp>

namespace HOD
{
    namespace PHYSIC
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Actor
        {
        public:
            Actor();
            ~Actor() = default;

			void	SetPosition(const glm::vec3& position);
			void	SetRotation(const glm::quat& rotation);

			physx::PxActor* GetPxActor() const;

        private:

            physx::PxActor* pxActor = nullptr;
        };
    }
}
