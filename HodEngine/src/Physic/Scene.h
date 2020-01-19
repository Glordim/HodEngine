#pragma once

#include <PxPhysicsAPI.h>

namespace HOD
{
    namespace Physic
    {
        class Scene
        {
        public:
            Scene() = default;
            ~Scene() = default;

        private:
            physx::PxScene* pxScene = nullptr;
        };
    }
}
