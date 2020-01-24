#include "Actor.h"

#include "Physic.h"

namespace HOD
{
    namespace PHYSIC
    {
		Actor::Actor(physx::PxActor* pxActor) : pxActor(pxActor)
		{
			
		}

		void Actor::SetPosition(const glm::vec3& position)
		{
			/*
physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
*/
		}

		void Actor::SetRotation(const glm::quat& rotation)
		{

		}

		physx::PxActor* Actor::GetPxActor() const
		{
			return pxActor;
		}
    }
}
