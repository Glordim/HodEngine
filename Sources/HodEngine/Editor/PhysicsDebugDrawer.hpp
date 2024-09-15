#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

namespace hod::renderer
{
	class Material;
	class MaterialInstance;
	class RenderQueue;
}

namespace hod::physics
{
	struct RenderCommand;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API PhysicsDebugDrawer
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

	class HOD_EDITOR_API RenderCommandPhysicsDrawer : public renderer::RenderCommandMesh
	{
	public:

								RenderCommandPhysicsDrawer(const physics::RenderCommand& renderCommand, const renderer::Material& material);
								RenderCommandPhysicsDrawer(const RenderCommandPhysicsDrawer&) = delete;
								RenderCommandPhysicsDrawer(RenderCommandPhysicsDrawer&&) = delete;
								~RenderCommandPhysicsDrawer() override = default;

		void					operator=(const RenderCommandPhysicsDrawer&) = delete;
		void					operator=(RenderCommandPhysicsDrawer&&) = delete;

	public:

		void					Execute(renderer::CommandBuffer* commandBuffer, renderer::MaterialInstance* overrideMaterial) override;
	};
}
