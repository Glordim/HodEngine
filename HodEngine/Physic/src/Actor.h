#pragma once

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/glm.hpp>

namespace physx
{
    class PxActor;
}

namespace HOD
{
    namespace PHYSIC
    {
        enum SHAPE
        {
            BOX,
            CYLINDER,
            CAPSULE,
            SPHERE
        };

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Actor
        {
        public:
            Actor(physx::PxActor* pxActor);
            ~Actor() = default;

            void    SetShape(SHAPE eShape);

            void    SetTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

			physx::PxActor* GetPxActor() const;

        private:

            physx::PxActor* pxActor = nullptr;
        };
    }
}
