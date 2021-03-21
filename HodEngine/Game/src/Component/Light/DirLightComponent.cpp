#include "DirLightComponent.h"

#include <ImGui/src/imgui.h>

#include "../../Actor.h"
#include "../StaticMeshComponent.h"

#include <Renderer/src/RHI/MaterialInstance.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DirLightComponent::DirLightComponent(Actor* actor) : Component(actor)
		{
			_data.intensity = 1.0f;
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
		void DirLightComponent::DrawImGui()
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
		RENDERER::DirLight* DirLightComponent::GetDirLight() const
		{
			return (RENDERER::DirLight*)(&_data);
		}
	}
}
