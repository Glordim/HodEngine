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
