#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>

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
					~ViewportWindow() override;

	public:

		void		DrawContent() override;

		void		OpenTab(std::shared_ptr<Asset> asset);

		void		MarkCurrentSceneAsDirty();

	private:

		class Tab final
		{
		public:
									Tab(std::shared_ptr<Asset> asset);
									Tab(const Tab&) = delete;
									Tab(Tab&&) = delete;
									~Tab();

			Tab&					operator = (const Tab&) = delete;
			Tab&					operator = (Tab&&) = delete;

		public:

			void						EnablePhysicsDebugDrawer(bool enabled);
			bool						IsPhysicsDebugDrawerEnabled(bool enabled) const;

		public:

			Vector2							_cameraPosition = Vector2::Zero;
			float							_size = 5.0f;
			game::Scene*					_scene = nullptr;
			game::Prefab*					_prefab = nullptr;
			std::shared_ptr<Asset>			_asset;

			PhysicsDebugDrawer*				_physicsDebugDrawer = nullptr;
		};

	private:

		void					DrawTab(Tab* tab);

		std::vector<Tab*>::iterator	CloseTab(std::vector<Tab*>::iterator it);

	private:
		
		renderer::RenderTarget* _renderTarget = nullptr;
		renderer::RenderTarget* _pickingRenderTarget = nullptr;
		ImGuizmo::OPERATION		_gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

		bool					_pickingRequest = false;
		Vector2					_pickingPosition = Vector2::Zero;

		std::vector<Tab*>		_tabs;
		Tab*					_selectedTab = nullptr;
		Tab*					_tabToSelect = nullptr;
	};
}
