#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline rhi
{
	class RenderTarget;
}

namespace hod::inline renderer
{
	class ReadbackRenderTarget;
	class RenderView;
}

namespace hod::inline editor
{
	class PhysicsDebugDrawer;

	/// @brief
	class HOD_GAME_EDITOR_API ViewportWindow : public EditorTabWindow, public GizmoViewport
	{
		REFLECTED_CLASS(ViewportWindow, EditorTabWindow)

	public:
		ViewportWindow() = default; // todo remove
		ViewportWindow(EditorTab* editorTab);
		~ViewportWindow() override;

	public:
		bool Draw() override;
		void DrawContent() override;

		RenderView*   GetRenderView() override;
		ReadbackRenderTarget* GetPickingRenderTarget() const override;
		const Matrix4&          GetProjectionMatrix() const override;
		const Matrix4&          GetViewMatrix() const override;
		float                   GetCameraSize() const override;

		const Vector2& GetPlayRatio() const override;

		/// @brief true while the mouse is used by the toolbar floating over the viewport, the viewport and its gizmos must then ignore it
		bool IsMouseOverToolbar() const override;

	private:
		void DrawToolbar();
		void DrawViewport();

		void EnablePhysicsDebugDrawer(bool enabled);
		bool IsPhysicsDebugDrawerEnabled() const;

		RenderView*   _renderView = nullptr;
		RenderTarget* _renderTarget = nullptr;
		ReadbackRenderTarget* _pickingRenderTarget = nullptr;

		Matrix4 _projection;
		Matrix4 _view;

		Vector2 _playRatio = Vector2(16.0f, 9.0f);

		Vector2 _cameraPosition = Vector2::Zero;
		float   _size = 5.0f;

		PhysicsDebugDrawer* _physicsDebugDrawer = nullptr;
		bool                _debugPicker = false;

		bool _drawGrid = true;

		bool _mouseOverToolbar = false;
		bool _canvasPointerDown = false; // Playing: a press started over the image, the game's canvases keep the mouse until released
	};
}
