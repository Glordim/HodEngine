#pragma once

#include "Debug/PhysicsDebugWindow.h"

#include <Core/src/Singleton.h>

#include <vector>

namespace physx
{
	class PxShape;
	class PxFoundation;
	class PxPhysics;
	class PxGeometry;
	class PxMaterial;

	class PxDefaultAllocator;
	class PxDefaultErrorCallback;
}

namespace HOD
{
	namespace PHYSICS
	{
		class Actor;
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Physics : public CORE::Singleton<Physics>
		{
			friend class CORE::Singleton<Physics>;

		protected:

											Physics();
											~Physics() override;

		public:

			bool							Init();
			void							Clear();

			const physx::PxMaterial&		GetDefaultMaterial() const;

			Scene*							CreateScene();
			void							DestroyScene(Scene* scene);

			Actor* CreateActor();

			physx::PxShape*					CreateShape(physx::PxGeometry& pxGeometry, physx::PxMaterial* pxMaterial = nullptr);

		private:

			physx::PxDefaultAllocator*		_pxDefaultAllocator = nullptr;
			physx::PxDefaultErrorCallback*	_pxDefaultErrorCallback = nullptr;

			physx::PxFoundation*			_pxFoundation = nullptr;
			physx::PxPhysics*				_pxPhysics = nullptr;

			physx::PxMaterial*				_pxDefaultMaterial = nullptr;

			std::vector<Scene*>				_scenes;

			// Debug
		public:

			void							SetShapeVisualizationFlag(bool visualization);
			void							SetActorVisualizationFlag(bool visualization);

			bool							GetShapeVisualizationFlag() const;
			bool							GetActorVisualizationFlag() const;

		private:

			bool							_visualizeShape = false;
			bool							_visualizeActor = false;

			PhysicsDebugWindow				_physicDebugWindow;
		};
	}
}
