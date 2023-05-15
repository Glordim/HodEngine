#include "DirLightComponent.h"

#include "../../Actor.h"
#include <HodEngine/Renderer/RHI/MaterialInstance.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DirLightComponent::DirLightComponent(Actor* actor) : Component(actor)
		{
			_data.intensity = 1.0f;
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
		void DirLightComponent::DrawImGui()
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
		}

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
