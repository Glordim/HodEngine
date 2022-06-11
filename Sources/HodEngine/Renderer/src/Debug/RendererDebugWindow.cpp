#include "RendererDebugWindow.h"
#include "../Renderer.h"

#include <HodEngine/ImGui/src/imgui.h>
#include <HodEngine/ImGui/src/ImGuizmo.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererDebugWindow::RendererDebugWindow() : DebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererDebugWindow::~RendererDebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RendererDebugWindow::Draw()
		{
			ImGui::Begin("Renderer");
			{
				Renderer* pRenderer = Renderer::GetInstance();

				Renderer::VisualizationMode visualizationMode = pRenderer->GetVisualizationMode();

				const char* visualizationModeLabel[] = { "Normal", "Normal + Wireframe", "Wireframe", "Overdraw" };
				if (ImGui::BeginCombo("VisualizationMode", visualizationModeLabel[visualizationMode]) == true)
				{
					for (uint32_t visualizationModeIndex = 0; visualizationModeIndex < Renderer::VisualizationMode::Count; ++visualizationModeIndex)
					{
						if (ImGui::Selectable(visualizationModeLabel[visualizationModeIndex]) == true)
						{
							pRenderer->SetVisualizationMode(static_cast<Renderer::VisualizationMode>(visualizationModeIndex));
						}
					}
					ImGui::EndCombo();
				}
			}
			ImGui::End();
		}
	}
}
