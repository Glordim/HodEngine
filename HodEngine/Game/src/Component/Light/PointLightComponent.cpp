#include "PointLightComponent.h"

#include <ImGui/src/imgui.h>

#include "../../Actor.h"
#include "../StaticMeshComponent.h"

#include <Renderer/src/MaterialInstance.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PointLightComponent::PointLightComponent(Actor* actor) : Component(actor)
		{
			_data.intensity = 1.0f;
			_data.range = 2.5f;
			_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);

			StaticMeshComponent* pStaticMesh = GetActor()->GetComponent<StaticMeshComponent>();
			if (pStaticMesh != nullptr)
			{
				pStaticMesh->GetMaterialInstance()->SetVec4("matUbo.color", glm::vec4(_data.color.r, _data.color.g, _data.color.b, _data.color.a));
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PointLightComponent::DrawImGui()
		{
			if (ImGui::ColorPicker3("Color", &_data.color[0]) == true)
			{
				StaticMeshComponent* pStaticMesh = GetActor()->GetComponent<StaticMeshComponent>();
				if (pStaticMesh != nullptr)
				{
					pStaticMesh->GetMaterialInstance()->SetVec4("matUbo.color", glm::vec4(_data.color.r, _data.color.g, _data.color.b, _data.color.a));
				}
			}
			ImGui::SliderFloat("Intensity", &_data.intensity, 0.0f, 100.0f);
			ImGui::SliderFloat("Range", &_data.range, 0.0f, 100.0f);
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
		RENDERER::PointLight* PointLightComponent::GetPointLight() const
		{
			return (RENDERER::PointLight*)(&_data);
		}
	}
}
