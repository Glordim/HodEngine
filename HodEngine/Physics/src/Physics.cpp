#include "Physics.h"

#include "Scene.h"
#include "Actor.h"

#include <DebugLayer/src/DebugLayer.h>

#include <PxPhysicsAPI.h>

namespace HOD
{
	PHYSICS::Physics* CORE::Singleton<PHYSICS::Physics>::_instance = nullptr;

	namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Physics::Physics() : Singleton()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Physics::~Physics()
		{
			Clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::Init()
		{
			_pxDefaultAllocator = new physx::PxDefaultAllocator();
			_pxDefaultErrorCallback = new physx::PxDefaultErrorCallback();

			_pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *_pxDefaultAllocator, *_pxDefaultErrorCallback);
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

			_pxDefaultMaterial = _pxPhysics->createMaterial(0.0f, 0.0f, 0.0f);
			if (_pxDefaultMaterial == nullptr)
			{
				return false;
			}

			DEBUG_LAYER::DebugLayer::GetInstance()->RegisterDebugWindow(&_physicDebugWindow);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::Clear()
		{
			DEBUG_LAYER::DebugLayer::GetInstance()->UnregisterDebugWindow(&_physicDebugWindow);

			if (_pxDefaultMaterial != nullptr)
			{
				_pxDefaultMaterial->release();
				_pxDefaultMaterial = nullptr;
			}

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

			if (_pxDefaultAllocator != nullptr)
			{
				delete _pxDefaultAllocator;
				_pxDefaultAllocator = nullptr;
			}

			if (_pxDefaultErrorCallback != nullptr)
			{
				delete _pxDefaultErrorCallback;
				_pxDefaultErrorCallback = nullptr;
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const physx::PxMaterial& Physics::GetDefaultMaterial() const
		{
			return *_pxDefaultMaterial;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene* Physics::CreateScene()
		{
			physx::PxTolerancesScale tolerancesScale = _pxPhysics->getTolerancesScale();

			physx::PxSceneDesc pxSceneDesc(tolerancesScale);

			physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
			pxSceneDesc.filterShader = gDefaultFilterShader;

			physx::PxCpuDispatcher* pxCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
			pxSceneDesc.cpuDispatcher = pxCpuDispatcher;

			physx::PxScene* pxScene = _pxPhysics->createScene(pxSceneDesc);
			pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
			pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			pxScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

			Scene* scene = new Scene(pxScene);

			_scenes.push_back(scene);

			return scene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::DestroyScene(Scene* scene)
		{
			auto it = _scenes.begin();
			auto itEnd = _scenes.end();
			while (it != itEnd)
			{
				if (*it == scene)
				{
					_scenes.erase(it);
					break;
				}
			}

			delete scene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor* Physics::CreateActor()
		{
			return new Actor(_pxPhysics->createRigidStatic(physx::PxTransform(physx::PxIDENTITY())));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		physx::PxShape* Physics::CreateShape(physx::PxGeometry& pxGeometry, physx::PxMaterial* pxMaterial)
		{
			if (pxMaterial == nullptr)
			{
				return _pxPhysics->createShape(pxGeometry, *_pxDefaultMaterial);
			}
			else
			{
				return _pxPhysics->createShape(pxGeometry, *pxMaterial);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::SetShapeVisualizationFlag(bool visualization)
		{
			_visualizeShape = visualization;

			for (Scene* scene : _scenes)
			{
				scene->ApplyShapeVisualizationFlag(visualization);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Physics::SetActorVisualizationFlag(bool visualization)
		{
			_visualizeActor = visualization;

			for (Scene* scene : _scenes)
			{
				scene->ApplyActorVisualizationFlag(visualization);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::GetShapeVisualizationFlag() const
		{
			return _visualizeShape;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Physics::GetActorVisualizationFlag() const
		{
			return _visualizeActor;
		}
	}
}
