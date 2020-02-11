#pragma once

#include "Debug/PhysicDebugWindow.h"

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
    namespace PHYSIC
    {
		class Actor;
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Physic : public CORE::Singleton<Physic>
        {
            friend class CORE::Singleton<Physic>;

        protected:

                                            Physic();
                                            ~Physic() override;

        public:

            bool                            Init();
            void                            Clear();

            const physx::PxMaterial&        GetDefaultMaterial() const;

			Scene*							CreateScene();
            void                            DestroyScene(Scene* pScene);

			Actor*							CreateActor();

            physx::PxShape*                 CreateShape(physx::PxGeometry& pPxGeometry, physx::PxMaterial* pPxMaterial = nullptr);

        private:

            physx::PxDefaultAllocator*      _pDefaultAllocator = nullptr;
            physx::PxDefaultErrorCallback*  _pDefaultErrorCallback = nullptr;

            physx::PxFoundation*            _pPxFoundation = nullptr;
            physx::PxPhysics*               _pPxPhysics = nullptr;

            physx::PxMaterial*              _pPxDefaultMaterial = nullptr;

            std::vector<Scene*>				_vScenes;

		// Debug
		public:

			void							SetShapeVisualizationFlag(bool bVisualizeShape);
			void							SetActorVisualizationFlag(bool bVisualizeActor);

			bool							GetShapeVisualizationFlag() const;
			bool							GetActorVisualizationFlag() const;

		private:

			bool							_bVisualizeShape = false;
			bool							_bVisualizeActor = false;

			PhysicDebugWindow				_physicDebugWindow;
        };
    }
}
