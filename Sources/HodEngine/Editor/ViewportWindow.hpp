#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/ImGuizmo.h>

namespace hod::renderer
{
	class RenderTarget;
}

namespace hod::game
{
	class Scene;
	class Prefab;
}

namespace hod::editor
{
	class Asset;
	class PhysicsDebugDrawer;

	/// @brief 
	class HOD_EDITOR_API ViewportWindow : public imgui::Window
	{
		META_TYPE(ViewportWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					ViewportWindow();
					ViewportWindow(std::shared_ptr<Asset> asset);
					~ViewportWindow() override;

	public:

		bool		Draw() override;
		void		DrawContent() override;

		void		MarkCurrentSceneAsDirty();

		std::shared_ptr<Asset> GetAsset() const;

		renderer::RenderTarget*		GetPickingRenderTarget() const;
		const Matrix4&				GetProjectionMatrix() const;
		const Matrix4&				GetViewMatrix() const;

	private:

		void		EnablePhysicsDebugDrawer(bool enabled);
		bool		IsPhysicsDebugDrawerEnabled(bool enabled) const;
		
		renderer::RenderTarget* 	_renderTarget = nullptr;
		renderer::RenderTarget* 	_pickingRenderTarget = nullptr;
		ImGuizmo::OPERATION			_gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

		Matrix4						_projection;
		Matrix4						_view;

		bool						_pickingRequest = false;
		Vector2						_pickingPosition = Vector2::Zero;

		Vector2						_cameraPosition = Vector2::Zero;
		float						_size = 5.0f;
		game::Scene*				_scene = nullptr;
		game::Prefab*				_prefab = nullptr;
		std::shared_ptr<Asset>		_asset;

		PhysicsDebugDrawer*			_physicsDebugDrawer = nullptr;
		bool						_debugPicker = false;
	};
}
