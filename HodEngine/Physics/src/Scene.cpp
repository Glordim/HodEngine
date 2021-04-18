#include "Scene.h"

#include "Actor.h"
#include "Physics.h"

#include "DebugDrawer.h"

#include <box2d/b2_world.h>
#include <box2d/b2_math.h>
#include <box2d/b2_body.h>

namespace HOD
{
	namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene::Scene()
		{
			_debugDrawer = new DebugDrawer();

			b2Vec2 defaultGravity(0.0f, 9.8f);

			_b2World = new b2World(defaultGravity);
			_b2World->SetDebugDraw(_debugDrawer);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene::~Scene()
		{
			delete _b2World;
			delete _debugDrawer;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor* Scene::CreateBody()
		{
			b2BodyDef bodyDef;

			Actor* actor = new Actor(_b2World->CreateBody(&bodyDef));

			_actors.push_back(actor);

			return actor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::Update(float dt)
		{
			_b2World->Step(dt, _velocityIterations, _positionIterations);

			if (_useDebugDraw == true)
			{
				_b2World->DebugDraw();
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Scene::Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance, PHYSICS::RaycastResult& result)
		{
			// TODO
			//_world->RayCast();
			return false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::GetDebugGeometry(std::vector<RENDERER::Line_3P_3C>& lines, std::vector<RENDERER::Tri_3P_3C>& tris)
		{
			/*
			const physx::PxRenderBuffer& rb = _pxScene->getRenderBuffer();
			physx::PxU32 lineCount = rb.getNbLines();

			if (lineCount != 0)
			{
				lines.resize(lineCount);

				for (physx::PxU32 i = 0; i < lineCount; i++)
				{
					const physx::PxDebugLine& pxLine = rb.getLines()[i];

					RENDERER::Line_3P_3C& line = lines[i];

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

					RENDERER::Tri_3P_3C& tri = tris[i];

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
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::SetDebugDraw(bool debugDraw)
		{
			_useDebugDraw = debugDraw;
		}
		
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Scene::SetDebugDrawFlags(DebugDrawFlag flag, bool enabled)
		{
			uint32_t mask = _debugDrawer->GetFlags();

			if (enabled == true)
			{
				mask |= (1 << flag);
			}
			else
			{
				mask &= ~(1 << flag);
			}

			_debugDrawer->SetFlags(mask);
		}
	}
}
