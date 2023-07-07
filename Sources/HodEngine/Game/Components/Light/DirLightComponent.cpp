#include "HodEngine/Game/Components/Light/DirLightComponent.h"

#include "HodEngine/Game/Actor.h"
#include <HodEngine/Renderer/RHI/MaterialInstance.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
			/*
		DirLightComponent::DirLightComponent(const std::weak_ptr<Entity>& entity) : Component(entity)
		{
			_data.intensity = 1.0f;
			_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);

			StaticMeshComponent* pStaticMesh = GetActor()->GetComponent<StaticMeshComponent>();
			if (pStaticMesh != nullptr)
			{
				pStaticMesh->GetMaterialInstance()->SetVec4("matUbo.color", glm::vec4(_data.color.r, _data.color.g, _data.color.b, _data.color.a));
			}
		}
			*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* DirLightComponent::GetType() const
		{
			return "Directional Light";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::DirLight* DirLightComponent::GetDirLight() const
		{
			return (renderer::DirLight*)(&_data);
		}
	}
}
