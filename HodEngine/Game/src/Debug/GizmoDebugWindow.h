#pragma once

#include <DebugLayer/src/DebugWindow.h>

#include <vector>
#include <map>

#include <ImGui/src/imgui.h>
#include <ImGui/src/ImGuizmo.h>

namespace HOD
{
    namespace GAME
	{
        class Actor;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class GizmoDebugWindow : public DEBUG_LAYER::DebugWindow
        {
		public:

											GizmoDebugWindow();
                                            ~GizmoDebugWindow() override;

            void                            Draw() override;

            void                            SetActor(Actor* pActor);

        private:

			Actor*							_pActor = nullptr;

			ImGuizmo::OPERATION				_eOperation = ImGuizmo::OPERATION::TRANSLATE;
			ImGuizmo::MODE					_eMode = ImGuizmo::MODE::LOCAL;
        };
    }
}
