#pragma once

namespace hod::renderer
{
	class MaterialInstance;
	class RenderQueue;
}
namespace hod::game
{
	/// @brief 
	class PhysicsDebugDrawer
	{
	public:
												PhysicsDebugDrawer();
												~PhysicsDebugDrawer() = default;

	public:

		void									PushToRenderQueue(renderer::RenderQueue& renderQueue);

	private:

		static renderer::MaterialInstance*		_pointMaterialInstance;
		static renderer::MaterialInstance*		_lineMaterialInstance;
		static renderer::MaterialInstance*		_wireframePolygonMaterialInstance;
		static renderer::MaterialInstance*		_solidPolygonMaterialInstance;
	};
}
