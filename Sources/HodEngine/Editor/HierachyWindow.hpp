#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/ImGui/Window/Window.hpp>
#include <memory>

namespace hod::game
{
	class Entity;
	class Component;
}

namespace hod::editor
{
	/// @brief 
	class HOD_API HierachyWindow : public imgui::Window
	{
		META_TYPE(HierachyWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					HierachyWindow();
					~HierachyWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		/*
		struct EntityNode
		{
			std::weak_ptr<game::Entity> _entity;
			bool						_collapsed = true;

			std::vector<EntityNode*>	_children;
			EntityNode*					_parent = nullptr;
		};
		*/

	private:

		//void		DrawEntityNode(EntityNode* entityNode);
		void		DrawEntity(std::weak_ptr<game::Entity> entity);

		void		OnAddEntityCallback(std::weak_ptr<game::Entity> entity);
		void		OnRemoveEntityCallback(std::weak_ptr<game::Entity> entity);
		void		OnRenameEntityCallback(std::weak_ptr<game::Entity> entity);

		void		OnAddComponentCallback(std::weak_ptr<game::Component> componenent);
		void		OnRemoveComponentCallback(std::weak_ptr<game::Component> componenent);

	private:

		std::weak_ptr<game::Entity>					_selection;
		bool										_openContextualMenu = false;

		//EntityNode									_rootEntityNode;
	};
}
