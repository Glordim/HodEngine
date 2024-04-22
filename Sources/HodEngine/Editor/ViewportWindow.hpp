#pragma once

#include <HodEngine/ImGui/Window.hpp>
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

	/// @brief 
	class ViewportWindow : public imgui::Window
	{
		META_TYPE(ViewportWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					ViewportWindow();
					~ViewportWindow() override;

	public:

		void		Draw() override;

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

			Vector2							_cameraPosition = Vector2::Zero;
			float							_size = 5.0f;
			game::Scene*					_scene = nullptr;
			game::Prefab*					_prefab = nullptr;
			std::shared_ptr<Asset>			_asset;
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
