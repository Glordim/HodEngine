#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/PhysicsDebugDrawer.hpp"

#include <HodEngine/Renderer/ShapeDrawer.hpp>

#include <HodEngine/Physics/DebugDrawer.hpp>
#include <HodEngine/Physics/World.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param world
	void PhysicsDebugDrawer::Update(physics::World* world)
	{
		world->GetDebugDrawer()->Update(world);
	}

	/// @brief
	/// @param renderView
	/// @param world
	void PhysicsDebugDrawer::PushRenderCommand(RenderView& renderView, physics::World* world)
	{
		constexpr uint32_t order = 0; // in place of the render of the world, in the order Box2d gives them (a fill then its outline)

		for (const physics::RenderCommand& shape : world->GetDebugDrawer()->GetRenderCommands())
		{
			const Vector2* points = shape._vertices.Data();
			uint32_t       pointCount = (uint32_t)shape._vertices.Size();

			switch (shape._type)
			{
				case physics::RenderCommand::Type::Point:
					if (pointCount > 0)
					{
						ShapeDrawer::Point(renderView, points[0], shape._size, shape._color, order);
					}
					break;
				case physics::RenderCommand::Type::Line: ShapeDrawer::Lines(renderView, Matrix4::Identity, points, pointCount, shape._color, order); break;
				case physics::RenderCommand::Type::WireframePolygon: ShapeDrawer::Polyline(renderView, Matrix4::Identity, points, pointCount, true, shape._color, order); break;
				case physics::RenderCommand::Type::FillPolygon: ShapeDrawer::FilledPolygon(renderView, Matrix4::Identity, points, pointCount, shape._color, order); break;
			}
		}
	}
}
