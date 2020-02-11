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
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor::Actor(physx::PxActor* pPxActor) : _pPxActor(pPxActor)
		{
			_pPxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, Physic::GetInstance()->GetActorVisualizationFlag());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        void Actor::SetShape(SHAPE eShape, const BoundingBox& boundingBox, const glm::vec3& scale)
        {
            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(_pPxActor);

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

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
		{
            physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));
            pxTransform.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
            //pxTransform.sca

            physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(_pPxActor);

            rigidActor->setGlobalPose(pxTransform);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		physx::PxActor* Actor::GetPxActor() const
		{
			return _pPxActor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetShapesVisualizationFlag(bool bShapeVisualization)
		{
			physx::PxRigidActor* pRigidActor = static_cast<physx::PxRigidActor*>(_pPxActor);

			physx::PxShape** aShapes = new physx::PxShape*[pRigidActor->getNbShapes()];

			size_t uiShapeCount = pRigidActor->getShapes(aShapes, pRigidActor->getNbShapes(), 0);

			for (size_t uiShapeIndex = 0; uiShapeIndex < uiShapeCount; ++uiShapeIndex)
			{
				pRigidActor->detachShape(*aShapes[uiShapeIndex]);

				aShapes[uiShapeIndex]->setFlag(physx::PxShapeFlag::eVISUALIZATION, bShapeVisualization);

				pRigidActor->attachShape(*aShapes[uiShapeIndex]);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetVisualizationFlag(bool bVisualization)
		{
			_pPxActor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, bVisualization);
		}
    }
}
