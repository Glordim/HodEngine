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

			Scene*							CreateScene();
			void							DestroyScene(Scene* scene);

			Actor*							CreateActor();

			physx::PxShape*					CreateShape(physx::PxGeometry& pxGeometry, physx::PxMaterial* pxMaterial = nullptr);

		private:

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
