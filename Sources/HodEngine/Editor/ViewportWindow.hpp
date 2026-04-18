#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline renderer
{
	class RenderTarget;
	class RenderView;
}

namespace hod::inline editor
{
	class PhysicsDebugDrawer;

	/// @brief
	class HOD_EDITOR_API ViewportWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(ViewportWindow, EditorTabWindow)

	public:
		ViewportWindow() = default; // todo remove
		ViewportWindow(EditorTab* editorTab);
		~ViewportWindow() override;

	public:
		bool Draw() override;
		void DrawContent() override;

		RenderView*   GetRenderView();
		RenderTarget* GetPickingRenderTarget() const;
		const Matrix4&          GetProjectionMatrix() const;
		const Matrix4&          GetViewMatrix() const;
		float                   GetCameraSize() const;

		const Vector2& GetPlayRatio() const;

	private:
		void EnablePhysicsDebugDrawer(bool enabled);
		bool IsPhysicsDebugDrawerEnabled() const;

		RenderView*   _renderView = nullptr;
		RenderTarget* _renderTarget = nullptr;
		RenderTarget* _pickingRenderTarget = nullptr;

		Matrix4 _projection;
		Matrix4 _view;

		Vector2 _playRatio = Vector2(16.0f, 9.0f);

		Vector2 _cameraPosition = Vector2::Zero;
		float   _size = 5.0f;

		PhysicsDebugDrawer* _physicsDebugDrawer = nullptr;
		bool                _debugPicker = false;

		bool _drawGrid = true;
	};
}
