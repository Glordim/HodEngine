#include "Actor.h"

#include "Physic.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <PxPhysicsAPI.h>

#include <Renderer/src/BoundingBox.h>

#include <algorithm>

namespace HOD
{
    namespace PHYSIC
    {
		Actor::Actor(physx::PxActor* pxActor) : pxActor(pxActor)
		{
            pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, Physic::GetInstance()->GetActorVisualizationFlag());
		}

        void Actor::SetShape(SHAPE eShape, const BoundingBox& boundingBox, const glm::vec3& scale)
        {
            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

            glm::vec3 halfSize = glm::max(glm::abs((boundingBox.max - boundingBox.min) * scale) , 0.001f) * 0.5f;

            physx::PxShape* pxShape = Physic::GetInstance()->CreateShape(physx::PxBoxGeometry(halfSize.x, halfSize.y, halfSize.z));
            if (pxShape != nullptr)
            {
                pxShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, Physic::GetInstance()->GetShapeVisualizationFlag());

                glm::vec3 pos = boundingBox.center * scale;

                pxShape->setLocalPose(physx::PxTransform(pos.x, pos.y, pos.z));

                rigidActor->attachShape(*pxShape);
            }
        }

		void Actor::SetTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
		{
            physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
            pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
            //pxTransform.sca

            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

            rigidActor->setGlobalPose(pxTransform);
		}

		physx::PxActor* Actor::GetPxActor() const
		{
			return pxActor;
		}

		void Actor::SetShapesVisualizationFlag(bool bShapeVisualization)
		{
			physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(pxActor);

			physx::PxShape** aShapes = new physx::PxShape*[rigidActor->getNbShapes()];

			size_t shapeCount = rigidActor->getShapes(aShapes, rigidActor->getNbShapes(), 0);

			for (size_t i = 0; i < shapeCount; ++i)
			{
				rigidActor->detachShape(*aShapes[i]);

				aShapes[i]->setFlag(physx::PxShapeFlag::eVISUALIZATION, bShapeVisualization);

				rigidActor->attachShape(*aShapes[i]);
			}
		}

		void Actor::SetVisualizationFlag(bool bVisualization)
		{
			pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, bVisualization);
		}
    }
}
