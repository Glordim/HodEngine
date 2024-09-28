#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"
#include "HodEngine/Core/Math/Vector4.hpp"

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
												~PhysicsDebugDrawer();

	public:

		void									PushToRenderQueue(renderer::RenderQueue& renderQueue);

	private:

		renderer::MaterialInstance*		_pointMaterialInstance = nullptr;
		renderer::MaterialInstance*		_lineMaterialInstance = nullptr;
		renderer::MaterialInstance*		_wireframePolygonMaterialInstance = nullptr;
		renderer::MaterialInstance*		_solidPolygonMaterialInstance = nullptr;
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

	private:

		const renderer::Material&	_material;
		Vector4						_color;
	};
}
