#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::renderer
{
	class RenderTarget;
	class RenderView;
}

namespace hod::editor
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

		renderer::RenderView*   GetRenderView();
		renderer::RenderTarget* GetPickingRenderTarget() const;
		const math::Matrix4&          GetProjectionMatrix() const;
		const math::Matrix4&          GetViewMatrix() const;
		float                   GetCameraSize() const;

		const math::Vector2& GetPlayRatio() const;

	private:
		void EnablePhysicsDebugDrawer(bool enabled);
		bool IsPhysicsDebugDrawerEnabled() const;

		renderer::RenderView*   _renderView = nullptr;
		renderer::RenderTarget* _renderTarget = nullptr;
		renderer::RenderTarget* _pickingRenderTarget = nullptr;

		math::Matrix4 _projection;
		math::Matrix4 _view;

		math::Vector2 _playRatio = math::Vector2(16.0f, 9.0f);

		math::Vector2 _cameraPosition = math::Vector2::Zero;
		float   _size = 5.0f;

		PhysicsDebugDrawer* _physicsDebugDrawer = nullptr;
		bool                _debugPicker = false;

		bool _drawGrid = true;
	};
}
