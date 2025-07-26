#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::renderer
{
	class RenderTarget;
}

namespace hod::editor
{
	class PhysicsDebugDrawer;
	
	/// @brief 
	class HOD_EDITOR_API ViewportWindow : public EditorTabWindow
	{
		META_TYPE(ViewportWindow, EditorTabWindow);
		WINDOW_DESCRIPTION()

	public:

					ViewportWindow() = default; // todo remove
					ViewportWindow(EditorTab* editorTab);
					~ViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

		renderer::RenderView*		GetRenderView();
		renderer::RenderTarget*		GetPickingRenderTarget() const;
		const Matrix4&				GetProjectionMatrix() const;
		const Matrix4&				GetViewMatrix() const;
		float						GetCameraSize() const;

		const Vector2&				GetPlayRatio() const;

	private:

		void		EnablePhysicsDebugDrawer(bool enabled);
		bool		IsPhysicsDebugDrawerEnabled() const;
		
		renderer::RenderTarget* 	_renderTarget = nullptr;
		renderer::RenderTarget* 	_pickingRenderTarget = nullptr;
		renderer::RenderView		_renderView;

		Matrix4						_projection;
		Matrix4						_view;

		Vector2						_playRatio = Vector2(16.0f, 9.0f);

		Vector2						_cameraPosition = Vector2::Zero;
		float						_size = 5.0f;

		PhysicsDebugDrawer*			_physicsDebugDrawer = nullptr;
		bool						_debugPicker = false;
		bool						_wasFocus = false;

		bool						_drawGrid = true;
	};
}
