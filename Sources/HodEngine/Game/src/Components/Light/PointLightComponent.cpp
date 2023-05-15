#include "PointLightComponent.h"

#include "../../Actor.h"
#include <HodEngine/Renderer/RHI/MaterialInstance.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PointLightComponent::PointLightComponent(Actor* actor) : Component(actor)
		{
			_data.intensity = 1.0f;
			_data.range = 2.5f;
			_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);

			/*
			StaticMeshComponent* pStaticMesh = GetActor()->GetComponent<StaticMeshComponent>();
			if (pStaticMesh != nullptr)
			{
				pStaticMesh->GetMaterialInstance()->SetVec4("matUbo.color", glm::vec4(_data.color.r, _data.color.g, _data.color.b, _data.color.a));
			}
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PointLightComponent::DrawImGui()
		{
			//if (ImGui::ColorPicker3("Color", &_data.color[0]) == true)
			{
				/*
				StaticMeshComponent* pStaticMesh = GetActor()->GetComponent<StaticMeshComponent>();
				if (pStaticMesh != nullptr)
				{
					pStaticMesh->GetMaterialInstance()->SetVec4("matUbo.color", glm::vec4(_data.color.r, _data.color.g, _data.color.b, _data.color.a));
				}
				*/
			}
			//ImGui::SliderFloat("Intensity", &_data.intensity, 0.0f, 100.0f);
			//ImGui::SliderFloat("Range", &_data.range, 0.0f, 100.0f);
		}

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
