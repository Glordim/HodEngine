#pragma once

#include <HodEngine/ImGui/Window.hpp>
#include <memory>

#include "HodEngine/Core/Event.hpp"

namespace hod::game
{
	class Entity;
	class Component;
}

namespace hod::editor
{
	/// @brief 
	class HierachyWindow : public imgui::Window
	{
		META_TYPE(HierachyWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					HierachyWindow();
					~HierachyWindow() override = default;

	public:

		void		Draw() override;

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

		//EntityNode									_rootEntityNode;

		Event<std::weak_ptr<game::Entity>>::Slot	_onAddEntityCallback;
		Event<std::weak_ptr<game::Entity>>::Slot	_onRemoveEntityCallback;
		Event<std::weak_ptr<game::Entity>>::Slot	_onRenameEntityCallback;

		Event<std::weak_ptr<game::Component>>::Slot	_onAddComponentCallback;
		Event<std::weak_ptr<game::Component>>::Slot	_onRemoveComponentCallback;
	};
}
