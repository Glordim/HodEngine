#pragma once

#include <PxPhysicsAPI.h>

namespace HOD
{
    namespace PHYSIC
    {
        class Actor
        {
        public:
            Actor() = default;
            ~Actor() = default;

        private:

            physx::PxActor* pxActor = nullptr;
        };
    }
}
