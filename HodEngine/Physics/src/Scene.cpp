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
			SetDebugDrawFlags(DebugDrawFlag::Shape, true);
			SetDebugDrawFlags(DebugDrawFlag::Join, true);
			SetDebugDrawFlags(DebugDrawFlag::AABB, true);
			SetDebugDrawFlags(DebugDrawFlag::CenterOfMass, true);
			SetDebugDrawFlags(DebugDrawFlag::Pair, true);
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
				assert(_debugDrawer->HasRenderCommand() == false);
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
		void Scene::PushToRenderQueue(RENDERER::RenderQueue& renderQueue)
		{
			if (_useDebugDraw == true)
			{
				_debugDrawer->PushToRenderQueue(renderQueue);
			}
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
