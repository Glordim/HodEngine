#pragma once


#include <vector>
#include <map>

#include <HodEngine/DebugLayer/src/DebugWindow.h>
#include <HodEngine/ImGui/src/imgui.h>
#include <HodEngine/ImGui/src/ImGuizmo.h>

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
									GizmoDebugWindow(const GizmoDebugWindow&) = delete;
									GizmoDebugWindow(GizmoDebugWindow&&) = delete;
									~GizmoDebugWindow() override;

			void					operator=(const GizmoDebugWindow&) = delete;
			void					operator=(GizmoDebugWindow&&) = delete;

		public:

			void					Draw() override;

			void					SetActor(Actor* actor);

		private:

			Actor*					_actor = nullptr;
			ImGuizmo::OPERATION		_operation = ImGuizmo::OPERATION::TRANSLATE;
			ImGuizmo::MODE			_mode = ImGuizmo::MODE::LOCAL;
		};
	}
}
