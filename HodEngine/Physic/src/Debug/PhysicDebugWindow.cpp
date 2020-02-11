#include "PhysicDebugWindow.h"
#include "Physic.h"

#include <ImGui/src/imgui.h>
#include <ImGui/src/ImGuizmo.h>

namespace HOD
{
    namespace PHYSIC
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PhysicDebugWindow::PhysicDebugWindow() : DebugWindow()
        {

        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PhysicDebugWindow::~PhysicDebugWindow()
		{
            
        }

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        void PhysicDebugWindow::Draw()
        {
            ImGui::Begin("Physic");
            {
				Physic* pPhycis = Physic::GetInstance();

				bool bVisualizeShape = pPhycis->GetShapeVisualizationFlag();
				bool bVisualizeActor = pPhycis->GetActorVisualizationFlag();

				if (ImGui::Checkbox("Visualize Shape", &bVisualizeShape) == true)
				{
					pPhycis->SetShapeVisualizationFlag(bVisualizeShape);
				}
				if (ImGui::Checkbox("Visualize Actor", &bVisualizeActor) == true)
				{
					pPhycis->SetActorVisualizationFlag(bVisualizeActor);
				}
            }
            ImGui::End();
        }
    }
}
