#include "HodEngine/Game/Components/Light/PointLightComponent.h"

#include <HodEngine/Renderer/RHI/MaterialInstance.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
			/*
		PointLightComponent::PointLightComponent(const std::weak_ptr<Entity>& entity) : Component(entity)
		{
			_data.intensity = 1.0f;
			_data.range = 2.5f;
			_data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);

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
		const char* PointLightComponent::GetType() const
		{
			return "Point Light";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::PointLight* PointLightComponent::GetPointLight() const
		{
			return (renderer::PointLight*)(&_data);
		}
	}
}
