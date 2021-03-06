#include "StaticMeshComponent.h"

#include <Renderer/src/Material.h>
#include <Renderer/src/Mesh.h>

#include "../Actor.h"

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		StaticMeshComponent::StaticMeshComponent(Actor* actor) : RendererComponent(actor)
		{
			_debugTangent = false;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		StaticMeshComponent::~StaticMeshComponent()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void StaticMeshComponent::DrawImGui()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* StaticMeshComponent::GetType() const
		{
			return "Static Mesh";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void StaticMeshComponent::SetMesh(RENDERER::Mesh* mesh)
		{
			_mesh = mesh;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RENDERER::Mesh* StaticMeshComponent::GetMesh() const
		{
			return _mesh;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void StaticMeshComponent::SetMaterialInstance(RENDERER::MaterialInstance* materialInstance)
		{
			_materialInstance = materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RENDERER::MaterialInstance* StaticMeshComponent::GetMaterialInstance() const
		{
			return _materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void StaticMeshComponent::EnableDebugTangent(bool enable)
		{
			_debugTangent = enable;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool StaticMeshComponent::IsDebugTangentEnabled() const
		{
			return _debugTangent;
		}
	}
}
