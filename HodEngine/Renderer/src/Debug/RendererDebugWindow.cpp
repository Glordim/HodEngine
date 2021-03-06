#include "RendererDebugWindow.h"
#include "../Renderer.h"

#include <ImGui/src/imgui.h>
#include <ImGui/src/ImGuizmo.h>

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

				bool bVisualize3D = pRenderer->GetVisualizationMode();

				if (ImGui::Checkbox("Visualize 3D", &bVisualize3D) == true)
				{
					pRenderer->SetVisualizationMode(bVisualize3D);
				}
			}
			ImGui::End();
		}
	}
}
