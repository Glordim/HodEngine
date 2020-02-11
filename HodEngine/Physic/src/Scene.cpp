#include "Scene.h"

#include "Actor.h"
#include "Physic.h"

#include <Renderer/src/Line_3P_3C.h>
#include <Renderer/src/Tri_3P_3C.h>

#include <PxPhysicsAPI.h>

namespace HOD
{
    namespace PHYSIC
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        Scene::Scene(physx::PxScene* pPxScene)
            : _pPxScene(pPxScene)
        {
            
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene::~Scene()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor* Scene::CreateActor()
		{
			Actor* pActor = Physic::GetInstance()->CreateActor();

			_pPxScene->addActor(*pActor->GetPxActor());

			_vActors.push_back(pActor);

			return pActor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::Update(float dt)
		{
			_pPxScene->simulate(dt);
			_pPxScene->fetchResults(true);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Scene::Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSIC::RaycastResult& result)
		{
			physx::PxVec3 pxOrigin(origin.x, origin.y, origin.z);
			physx::PxVec3 pxDir(dir.x, dir.y, dir.z);

			pxDir.normalize();

			physx::PxRaycastBuffer pxRaycastBuffer;

			bool bHit = _pPxScene->raycast(pxOrigin, pxDir, distance, pxRaycastBuffer);

			if (bHit == true)
			{
				// Fill raycast result
                physx::PxActor* pxActor = pxRaycastBuffer.block.actor;

                for (Actor* pActor : _vActors)
                {
                    if (pActor->GetPxActor() == pxActor)
                    {
                        result._pActorCollided = pActor;
                        break;
                    }
                }
			}

			return bHit;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::GetDebugGeometry(std::vector<Line_3P_3C>& lines, std::vector<Tri_3P_3C>& tris)
		{
			const physx::PxRenderBuffer& rb = _pPxScene->getRenderBuffer();
			physx::PxU32 lineCount = rb.getNbLines();

			if (lineCount != 0)
			{
				lines.resize(lineCount);

				for (physx::PxU32 i = 0; i < lineCount; i++)
				{
					const physx::PxDebugLine& pxLine = rb.getLines()[i];

					Line_3P_3C& line = lines[i];

					line.vertices[0].pos[0] = pxLine.pos0.x;
					line.vertices[0].pos[1] = pxLine.pos0.y;
					line.vertices[0].pos[2] = pxLine.pos0.z;
					line.vertices[0].color[0] = (float)((pxLine.color0 & 0x00FF0000) >> 16);
					line.vertices[0].color[1] = (float)((pxLine.color0 & 0x0000FF00) >> 8);
					line.vertices[0].color[2] = (float)((pxLine.color0 & 0x000000FF) >> 0);

					line.vertices[1].pos[0] = pxLine.pos1.x;
					line.vertices[1].pos[1] = pxLine.pos1.y;
					line.vertices[1].pos[2] = pxLine.pos1.z;
					line.vertices[1].color[0] = (float)((pxLine.color1 & 0x00FF0000) >> 16);
					line.vertices[1].color[1] = (float)((pxLine.color1 & 0x0000FF00) >> 8);
					line.vertices[1].color[2] = (float)((pxLine.color1 & 0x000000FF) >> 0);
				}
			}

			physx::PxU32 triCount = rb.getNbTriangles();

			if (triCount != 0)
			{
				tris.resize(triCount);

				for (physx::PxU32 i = 0; i < triCount; i++)
				{
					const physx::PxDebugTriangle& pxTri = rb.getTriangles()[i];
					// render the line

					Tri_3P_3C& tri = tris[i];

					tri.vertices[2].pos[0] = pxTri.pos0.x;
					tri.vertices[2].pos[1] = pxTri.pos0.y;
					tri.vertices[2].pos[2] = pxTri.pos0.z;
					tri.vertices[2].color[0] = (float)((pxTri.color0 & 0x00FF0000) >> 16);
					tri.vertices[2].color[1] = (float)((pxTri.color0 & 0x0000FF00) >> 8);
					tri.vertices[2].color[2] = (float)((pxTri.color0 & 0x000000FF) >> 0);

					tri.vertices[1].pos[0] = pxTri.pos1.x;
					tri.vertices[1].pos[1] = pxTri.pos1.y;
					tri.vertices[1].pos[2] = pxTri.pos1.z;
					tri.vertices[1].color[0] = (float)((pxTri.color1 & 0x00FF0000) >> 16);
					tri.vertices[1].color[1] = (float)((pxTri.color1 & 0x0000FF00) >> 8);
					tri.vertices[1].color[2] = (float)((pxTri.color1 & 0x000000FF) >> 0);

					tri.vertices[0].pos[0] = pxTri.pos2.x;
					tri.vertices[0].pos[1] = pxTri.pos2.y;
					tri.vertices[0].pos[2] = pxTri.pos2.z;
					tri.vertices[0].color[0] = (float)((pxTri.color2 & 0x00FF0000) >> 16);
					tri.vertices[0].color[1] = (float)((pxTri.color2 & 0x0000FF00) >> 8);
					tri.vertices[0].color[2] = (float)((pxTri.color2 & 0x000000FF) >> 0);
				}
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::ApplyShapeVisualizationFlag(bool bVisualizeShape)
		{
			for (Actor* pActor : _vActors)
			{
				pActor->SetShapesVisualizationFlag(bVisualizeShape);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::ApplyActorVisualizationFlag(bool bVisualizeActor)
		{
			for (Actor* pActor : _vActors)
			{
				pActor->SetVisualizationFlag(bVisualizeActor);
			}
		}
    }
}
