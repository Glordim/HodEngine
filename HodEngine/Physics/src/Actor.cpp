#include "Actor.h"

#include "Physics.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <PxPhysicsAPI.h>

#include <Renderer/src/BoundingBox.h>

#include <algorithm>

namespace HOD
{
    namespace PHYSICS
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor::Actor(physx::PxActor* pxActor) : _pxActor(pxActor)
		{
			_pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, Physics::GetInstance()->GetActorVisualizationFlag());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        void Actor::SetShape(SHAPE shape, const BoundingBox& boundingBox, const glm::vec3& scale)
        {
            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(_pxActor);

            glm::vec3 halfSize = glm::max(glm::abs((boundingBox.max - boundingBox.min) * scale) , 0.001f) * 0.5f;

            physx::PxShape* pxShape = Physics::GetInstance()->CreateShape(physx::PxBoxGeometry(halfSize.x, halfSize.y, halfSize.z));
            if (pxShape != nullptr)
            {
                pxShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, Physics::GetInstance()->GetShapeVisualizationFlag());

                glm::vec3 pos = boundingBox.center * scale;

                pxShape->setLocalPose(physx::PxTransform(pos.x, pos.y, pos.z));

                rigidActor->attachShape(*pxShape);
            }
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
		{
            physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
            pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
            //pxTransform.sca

            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(_pxActor);

            rigidActor->setGlobalPose(pxTransform);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		physx::PxActor* Actor::GetPxActor() const
		{
			return _pxActor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetShapesVisualizationFlag(bool visualization)
		{
			physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(_pxActor);

			physx::PxShape** shapes = new physx::PxShape*[rigidActor->getNbShapes()];

			size_t shapeCount = rigidActor->getShapes(shapes, rigidActor->getNbShapes(), 0);

			for (size_t shapeIndex = 0; shapeIndex < shapeCount; ++shapeIndex)
			{
				rigidActor->detachShape(*shapes[shapeIndex]);

				shapes[shapeIndex]->setFlag(physx::PxShapeFlag::eVISUALIZATION, visualization);

				rigidActor->attachShape(*shapes[shapeIndex]);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetVisualizationFlag(bool visualization)
		{
			_pxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, visualization);
		}
    }
}
