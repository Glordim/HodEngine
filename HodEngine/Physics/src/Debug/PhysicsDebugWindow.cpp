#include "PhysicsDebugWindow.h"
#include "../Physics.h"

#include <ImGui/src/imgui.h>
#include <ImGui/src/ImGuizmo.h>

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
