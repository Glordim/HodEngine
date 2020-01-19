#include "Physic.h"

namespace HOD
{
    PHYSIC::Physic* Singleton<PHYSIC::Physic>::_instance = nullptr;

    namespace PHYSIC
    {
        Physic::Physic() : Singleton()
        {

        }

        Physic::~Physic()
        {
            Clear();
        }

        bool Physic::Init()
        {
            _pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _defaultAllocator, _defaultErrorCallback);
            if (_pxFoundation == nullptr)
            {
                return false;
            }

            physx::PxTolerancesScale tolerancesScale;

            _pxPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *_pxFoundation, tolerancesScale);
            if (_pxPhysics == nullptr)
            {
                return false;
            }

            return true;
        }

        void Physic::Clear()
        {
            if (_pxPhysics != nullptr)
            {
                _pxPhysics->release();
                _pxPhysics = nullptr;
            }

            if (_pxFoundation != nullptr)
            {
                _pxFoundation->release();
                _pxFoundation = nullptr;
            }
        }
    }
}
