#include "Physic.h"

#include "Scene.h"
#include "Actor.h"

#include <DebugLayer/src/DebugLayer.h>

#include <PxPhysicsAPI.h>

namespace HOD
{
    PHYSIC::Physic* CORE::Singleton<PHYSIC::Physic>::_instance = nullptr;

    namespace PHYSIC
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        Physic::Physic() : Singleton()
        {

        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        Physic::~Physic()
        {
            Clear();
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        bool Physic::Init()
        {
            _pDefaultAllocator = new physx::PxDefaultAllocator();
			_pDefaultErrorCallback = new physx::PxDefaultErrorCallback();

            _pPxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *_pDefaultAllocator, *_pDefaultErrorCallback);
            if (_pPxFoundation == nullptr)
            {
                return false;
            }

            physx::PxTolerancesScale tolerancesScale;

            _pPxPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *_pPxFoundation, tolerancesScale);
            if (_pPxPhysics == nullptr)
            {
                return false;
            }

            _pPxDefaultMaterial = _pPxPhysics->createMaterial(0.0f, 0.0f, 0.0f);
            if (_pPxDefaultMaterial == nullptr)
            {
                return false;
            }

			DEBUG_LAYER::DebugLayer::GetInstance()->RegisterDebugWindow(&_physicDebugWindow);

            return true;
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        void Physic::Clear()
        {
			DEBUG_LAYER::DebugLayer::GetInstance()->UnregisterDebugWindow(&_physicDebugWindow);

            if (_pPxDefaultMaterial != nullptr)
            {
				_pPxDefaultMaterial->release();
				_pPxDefaultMaterial = nullptr;
            }

            if (_pPxPhysics != nullptr)
            {
				_pPxPhysics->release();
				_pPxPhysics = nullptr;
            }

            if (_pPxFoundation != nullptr)
            {
				_pPxFoundation->release();
				_pPxFoundation = nullptr;
            }

            if (_pDefaultAllocator != nullptr)
            {
                delete _pDefaultAllocator;
				_pDefaultAllocator = nullptr;
            }

            if (_pDefaultErrorCallback != nullptr)
            {
                delete _pDefaultErrorCallback;
				_pDefaultErrorCallback = nullptr;
            }
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        const physx::PxMaterial& Physic::GetDefaultMaterial() const
        {
            return *_pPxDefaultMaterial;
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene* Physic::CreateScene()
		{
            physx::PxTolerancesScale tolerancesScale = _pPxPhysics->getTolerancesScale();

            physx::PxSceneDesc pxSceneDesc(tolerancesScale);

            physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
            pxSceneDesc.filterShader = gDefaultFilterShader;

            physx::PxCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
            pxSceneDesc.cpuDispatcher = mCpuDispatcher;

            physx::PxScene* pxScene = _pPxPhysics->createScene(pxSceneDesc);
            pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
            pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
            pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

            Scene* pScene = new Scene(pxScene);

            _vScenes.push_back(pScene);

            return pScene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor* Physic::CreateActor()
		{
			return new Actor(_pPxPhysics->createRigidStatic(physx::PxTransform(physx::PxIDENTITY())));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        physx::PxShape* Physic::CreateShape(physx::PxGeometry& pPxGeometry, physx::PxMaterial* pPxMaterial)
        {
            if (pPxMaterial == nullptr)
            {
                return _pPxPhysics->createShape(pPxGeometry, *_pPxDefaultMaterial);
            }
            else
            {
                return _pPxPhysics->createShape(pPxGeometry, *pPxMaterial);
            }
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physic::SetShapeVisualizationFlag(bool bVisualizeShape)
		{
			_bVisualizeShape = bVisualizeShape;

			for (Scene* pScene : _vScenes)
			{
				pScene->ApplyShapeVisualizationFlag(bVisualizeShape);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physic::SetActorVisualizationFlag(bool bVisualizeActor)
		{
			_bVisualizeActor = bVisualizeActor;

			for (Scene* pScene : _vScenes)
			{
				pScene->ApplyActorVisualizationFlag(bVisualizeActor);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physic::GetShapeVisualizationFlag() const
		{
			return _bVisualizeShape;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physic::GetActorVisualizationFlag() const
		{
			return _bVisualizeActor;
		}
    }
}
