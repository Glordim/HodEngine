#include "Physic.h"

#include "Scene.h"

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

		void Physic::CreateScene()
		{
			physx::PxTolerancesScale tolerancesScale = _pxPhysics->getTolerancesScale();

			physx::PxSceneDesc pxSceneDesc(tolerancesScale);

			physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
			pxSceneDesc.filterShader = gDefaultFilterShader;

			physx::PxCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
			pxSceneDesc.cpuDispatcher = mCpuDispatcher;

			Scene* scene = _pxPhysics->createScene(pxSceneDesc);
			scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
			scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

			this->pxDefaultMaterial = pxPhysx.createMaterial(0.0f, 0.0f, 0.0f);
		}
    }
}
