#include "PhysicsDebugWindow.h"
#include "../Physics.h"

#include <HodEngine/ImGui/src/imgui.h>
#include <HodEngine/ImGui/src/ImGuizmo.h>

namespace HOD
{
	namespace PHYSICS
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PhysicsDebugWindow::PhysicsDebugWindow() : DebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PhysicsDebugWindow::~PhysicsDebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugWindow::Draw()
		{
			ImGui::Begin("Physics");
			{
				Physics* physics = Physics::GetInstance();

				
			}
			ImGui::End();
		}
	}
}
