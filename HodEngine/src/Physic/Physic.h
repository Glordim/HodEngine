#pragma once

#include "../Singleton.h"

#include <PxPhysicsAPI.h>

#include <vector>

namespace HOD
{
    namespace PHYSIC
    {
		class Actor;
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class Physic : public Singleton<Physic>
        {
            friend class Singleton<Physic>;

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

            physx::PxDefaultAllocator       _defaultAllocator;
            physx::PxDefaultErrorCallback   _defaultErrorCallback;

            physx::PxFoundation*            _pxFoundation = nullptr;
            physx::PxPhysics*               _pxPhysics = nullptr;

            physx::PxMaterial*              _pxDefaultMaterial = nullptr;

            std::vector<Scene*>				_vScenes;
        };
    }
}
