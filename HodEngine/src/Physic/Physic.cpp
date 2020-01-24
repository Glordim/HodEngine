#include "Physic.h"

#include "Scene.h"
#include "Actor.h"

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

		Scene* Physic::CreateScene()
		{
            physx::PxTolerancesScale tolerancesScale = _pxPhysics->getTolerancesScale();

            physx::PxSceneDesc pxSceneDesc(tolerancesScale);

            physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
            pxSceneDesc.filterShader = gDefaultFilterShader;

            physx::PxCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
            pxSceneDesc.cpuDispatcher = mCpuDispatcher;

            physx::PxScene* pxScene = _pxPhysics->createScene(pxSceneDesc);
            pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
            pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
            pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

            Scene* pScene = new Scene(pxScene);

            _vScenes.push_back(pScene);

            return pScene;
		}

        void Physic::DestroyScene(Scene* pScene)
        {
            auto it = _vScenes.begin();
            auto itEnd = _vScenes.end();
            while (it != itEnd)
            {
                if (*it == pScene)
                {
                    _vScenes.erase(it);
                    break;
                }
            }

            delete pScene;
        }

		Actor* Physic::CreateActor()
		{
			return new Actor(_pxPhysics->createRigidStatic(physx::PxTransform(physx::PxIDENTITY())));
		}
    }
}
