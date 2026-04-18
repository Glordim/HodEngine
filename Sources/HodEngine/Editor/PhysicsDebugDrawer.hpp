#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"
#include "HodEngine/Math/Vector4.hpp"

namespace hod::inline renderer
{
	class Material;
	class MaterialInstance;
	class RenderView;
}

namespace hod::inline physics
{
	struct RenderCommand;
	class World;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_EDITOR_API PhysicsDebugDrawer
	{
	public:
												PhysicsDebugDrawer();
												~PhysicsDebugDrawer();

	public:

		void									Update(physics::World* world);
		void									PushRenderCommand(RenderView& renderView, physics::World* world);

	private:

		MaterialInstance*		_pointMaterialInstance = nullptr;
		MaterialInstance*		_lineMaterialInstance = nullptr;
		MaterialInstance*		_wireframePolygonMaterialInstance = nullptr;
		MaterialInstance*		_solidPolygonMaterialInstance = nullptr;
	};

	class HOD_EDITOR_API RenderCommandPhysicsDrawer : public RenderCommandMesh
	{
	public:

								RenderCommandPhysicsDrawer(const physics::RenderCommand& renderCommand, const Material& material);
								RenderCommandPhysicsDrawer(const RenderCommandPhysicsDrawer&) = delete;
								RenderCommandPhysicsDrawer(RenderCommandPhysicsDrawer&&) = delete;
								~RenderCommandPhysicsDrawer() override = default;

		void					operator=(const RenderCommandPhysicsDrawer&) = delete;
		void					operator=(RenderCommandPhysicsDrawer&&) = delete;

	public:

		void					Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial) override;

	private:

		const Material&	_material;
		Vector4						_color;
	};
}
