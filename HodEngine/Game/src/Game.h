#pragma once

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

            physx::PxDefaultAllocator*      _defaultAllocator = nullptr;
            physx::PxDefaultErrorCallback*  _defaultErrorCallback = nullptr;

            physx::PxFoundation*            _pxFoundation = nullptr;
            physx::PxPhysics*               _pxPhysics = nullptr;

            physx::PxMaterial*              _pxDefaultMaterial = nullptr;

            std::vector<Scene*>				_vScenes;
        };
    }
}
