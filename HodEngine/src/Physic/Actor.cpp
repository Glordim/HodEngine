#include "Actor.h"

#include "Physic.h"

#include <glm/glm.hpp>

namespace HOD
{
    namespace PHYSIC
    {
		Actor::Actor(physx::PxActor* pxActor) : pxActor(pxActor)
		{
            pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
		}

        void Actor::SetShape(SHAPE eShape)
        {
            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

            physx::PxShape* pxShape = Physic::GetInstance()->CreateShape(physx::PxBoxGeometry(0.1f, 0.1f, 0.1f));
            pxShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

            rigidActor->attachShape(*pxShape);
        }

		void Actor::SetTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
		{
            physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
            //pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
            //pxTransform.sca

            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

            rigidActor->setGlobalPose(pxTransform);
		}

		physx::PxActor* Actor::GetPxActor() const
		{
			return pxActor;
		}
    }
}
