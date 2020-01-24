#include "Scene.h"

#include "Actor.h"

#include "Physic.h"

#include "Renderer/Line_3P_3C.h"
#include "Renderer/Tri_3P_3C.h"
#include "Color.h"

namespace HOD
{
    namespace PHYSIC
    {
        Scene::Scene(physx::PxScene* pPxScene)
            : pxScene(pPxScene)
        {
            
        }

		Scene::~Scene()
		{

		}

		Actor* Scene::CreateActor()
		{
			Actor* actor = Physic::GetInstance()->CreateActor();

			pxScene->addActor(*actor->GetPxActor());

			actors.push_back(actor);

			return actor;
		}

		void Scene::Update(float dt)
		{
			this->pxScene->simulate(dt);
			this->pxScene->fetchResults(true);
		}

		bool Scene::Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSIC::RaycastResult& result)
		{
			physx::PxVec3 pxOrigin(origin.x, origin.y, origin.z);
			physx::PxVec3 pxDir(dir.x, dir.y, dir.z);

			pxDir.normalize();

			physx::PxRaycastBuffer pxRaycastBuffer;

			bool bHit = pxScene->raycast(pxOrigin, pxDir, distance, pxRaycastBuffer);

			if (bHit == true)
			{
				// Fill raycast result
			}

			return bHit;
		}

		void Scene::GetDebugGeometry(std::vector<Line_3P_3C>& lines, std::vector<Tri_3P_3C>& tris)
		{
			const physx::PxRenderBuffer& rb = this->pxScene->getRenderBuffer();
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

					tri.vertices[0].pos[0] = pxTri.pos0.x;
					tri.vertices[0].pos[1] = pxTri.pos0.y;
					tri.vertices[0].pos[2] = pxTri.pos0.z;
					tri.vertices[0].color[0] = (float)((pxTri.color0 & 0x00FF0000) >> 16);
					tri.vertices[0].color[1] = (float)((pxTri.color0 & 0x0000FF00) >> 8);
					tri.vertices[0].color[2] = (float)((pxTri.color0 & 0x000000FF) >> 0);

					tri.vertices[1].pos[0] = pxTri.pos1.x;
					tri.vertices[1].pos[1] = pxTri.pos1.y;
					tri.vertices[1].pos[2] = pxTri.pos1.z;
					tri.vertices[1].color[0] = (float)((pxTri.color1 & 0x00FF0000) >> 16);
					tri.vertices[1].color[1] = (float)((pxTri.color1 & 0x0000FF00) >> 8);
					tri.vertices[1].color[2] = (float)((pxTri.color1 & 0x000000FF) >> 0);

					tri.vertices[2].pos[0] = pxTri.pos2.x;
					tri.vertices[2].pos[1] = pxTri.pos2.y;
					tri.vertices[2].pos[2] = pxTri.pos2.z;
					tri.vertices[2].color[0] = (float)((pxTri.color2 & 0x00FF0000) >> 16);
					tri.vertices[2].color[1] = (float)((pxTri.color2 & 0x0000FF00) >> 8);
					tri.vertices[2].color[2] = (float)((pxTri.color2 & 0x000000FF) >> 0);
				}
			}
		}
    }
}
