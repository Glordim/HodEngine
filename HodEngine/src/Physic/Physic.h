#pragma once

#include "../Singleton.h"

#include <PxPhysicsAPI.h>

namespace HOD
{
    namespace PHYSIC
    {
        class Physic : public Singleton<Physic>
        {
            friend class Singleton<Physic>;

        protected:

                                            Physic();
                                            ~Physic() override;

        public:

            bool                            Init();
            void                            Clear();

        private:

            physx::PxDefaultAllocator       _defaultAllocator;
            physx::PxDefaultErrorCallback   _defaultErrorCallback;

            physx::PxFoundation*            _pxFoundation = nullptr;
            physx::PxPhysics*               _pxPhysics = nullptr;
        };
    }
}
