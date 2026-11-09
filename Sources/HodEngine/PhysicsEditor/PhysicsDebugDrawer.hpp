#pragma once
#include "HodEngine/PhysicsEditor/Export.hpp"

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline physics
{
	class World;
}

namespace hod::inline editor
{
	/// @brief Draws the debug shapes of a physics World (physics::DebugDrawer) with renderer::ShapeDrawer
	class HOD_PHYSICS_EDITOR_API PhysicsDebugDrawer
	{
	public:
		void Update(physics::World* world);
		void PushRenderCommand(RenderView& renderView, physics::World* world);
	};
}
