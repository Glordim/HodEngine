#include "HodEngine/Editor/Pch.hpp"
#include "PhysicsDebugDrawer.hpp"

#include "HodEngine/Renderer/P2fC4f.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderView.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Core/Math/Math.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/World.hpp>
#include <HodEngine/Physics/DebugDrawer.hpp>

namespace hod::editor
{
	/// @brief 
	PhysicsDebugDrawer::PhysicsDebugDrawer()
	{
		renderer::MaterialManager* materialManager = renderer::MaterialManager::GetInstance();

		const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_TriangleFan);
		_solidPolygonMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);

		material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_TriangleFan);
		_wireframePolygonMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);

		material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line);
		_lineMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
	}

	/// @brief 
	PhysicsDebugDrawer::~PhysicsDebugDrawer()
	{
		DefaultAllocator::GetInstance().Delete(_solidPolygonMaterialInstance);
		DefaultAllocator::GetInstance().Delete(_wireframePolygonMaterialInstance);
		DefaultAllocator::GetInstance().Delete(_lineMaterialInstance);
		DefaultAllocator::GetInstance().Delete(_pointMaterialInstance);
	}

	/// @brief 
	/// @param world 
	void PhysicsDebugDrawer::Update(physics::World* world)
	{
		world->GetDebugDrawer()->Update(world);
	}

	/// @brief 
	/// @param renderQueue 
	void PhysicsDebugDrawer::PushRenderCommand(renderer::RenderView& renderView, physics::World* world)
	{
		for (const hod::physics::RenderCommand& renderCommand : world->GetDebugDrawer()->GetRenderCommands())
		{
			renderer::MaterialInstance* materialInstance = nullptr;

			if (renderCommand._type == hod::physics::RenderCommand::Type::Point)
			{
				materialInstance = PhysicsDebugDrawer::_pointMaterialInstance;
			}
			else if (renderCommand._type == hod::physics::RenderCommand::Type::Line)
			{
				materialInstance = PhysicsDebugDrawer::_lineMaterialInstance;
			}
			else if (renderCommand._type == hod::physics::RenderCommand::Type::WireframePolygon)
			{
				materialInstance = PhysicsDebugDrawer::_wireframePolygonMaterialInstance;
			}
			else if (renderCommand._type == hod::physics::RenderCommand::Type::FillPolygon)
			{
				materialInstance = PhysicsDebugDrawer::_solidPolygonMaterialInstance;
			}

			renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandPhysicsDrawer>(renderCommand, materialInstance->GetMaterial()));
		}
	}

	/// @brief 
	RenderCommandPhysicsDrawer::RenderCommandPhysicsDrawer(const hod::physics::RenderCommand& renderCommand, const renderer::Material& material)
	: RenderCommandMesh(renderCommand._vertices.data(), nullptr, nullptr, (uint32_t)renderCommand._vertices.size(), nullptr, 0, Matrix4::Identity, nullptr, true)
	, _material(material)
	, _color(renderCommand._color.r, renderCommand._color.g, renderCommand._color.b, renderCommand._color.a)
	{
	}

	/// @brief 
	/// @param commandBuffer 
	void RenderCommandPhysicsDrawer::Execute(renderer::CommandBuffer* commandBuffer, renderer::MaterialInstance* overrideMaterial)
	{
		if (overrideMaterial != nullptr)
		{
			return;
		}

		_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(&_material);
		const_cast<renderer::MaterialInstance*>(_materialInstance)->SetVec4("ubo.color", _color);
		RenderCommandMesh::Execute(commandBuffer, overrideMaterial);
		commandBuffer->DeleteAfterRender(const_cast<renderer::MaterialInstance*>(_materialInstance));
	}
}
