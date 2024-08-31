#include "HodEngine/Game/Pch.hpp"
#include "PhysicsDebugDrawer.hpp"

#include "HodEngine/Renderer/P2fC4f.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Core/Math/Math.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/DebugDrawer.hpp>

namespace hod::game
{
	renderer::MaterialInstance* PhysicsDebugDrawer::_pointMaterialInstance = nullptr;
	renderer::MaterialInstance* PhysicsDebugDrawer::_lineMaterialInstance = nullptr;
	renderer::MaterialInstance* PhysicsDebugDrawer::_wireframePolygonMaterialInstance = nullptr;
	renderer::MaterialInstance* PhysicsDebugDrawer::_solidPolygonMaterialInstance = nullptr;

	/// @brief 
	PhysicsDebugDrawer::PhysicsDebugDrawer()
	{
		renderer::MaterialManager* materialManager = renderer::MaterialManager::GetInstance();

		if (PhysicsDebugDrawer::_solidPolygonMaterialInstance == nullptr)
		{
			const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Fill_TriangleFan);
			_solidPolygonMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
		}
		if (PhysicsDebugDrawer::_wireframePolygonMaterialInstance == nullptr)
		{
			const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_TriangleFan);
			_wireframePolygonMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
		}
		if (PhysicsDebugDrawer::_lineMaterialInstance == nullptr)
		{
			const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_Line);
			_lineMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
		}
	}

	/// @brief 
	/// @param renderQueue 
	void PhysicsDebugDrawer::PushToRenderQueue(renderer::RenderQueue& renderQueue)
	{
		physics::Physics::GetInstance()->GetDebugDrawer()->Update();
		for (const hod::physics::RenderCommand& renderCommand : physics::Physics::GetInstance()->GetDebugDrawer()->GetRenderCommands())
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

			renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(renderCommand._vertices.data(), (uint32_t)renderCommand._vertices.size(), sizeof(float) * 2, nullptr, 0, Matrix4::Identity, materialInstance, true));
		}
	}
}
