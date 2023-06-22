#pragma once

#include <HodEngine/ImGui/Window.h>

namespace hod::game
{
	class Entity;
}

namespace hod::editor
{
	/// @brief 
	class HierachyWindow : public imgui::Window
	{
		META_TYPE(HierachyWindow);
		WINDOW_DESCRIPTION()

	public:

					HierachyWindow();
					~HierachyWindow() override = default;

	public:

		void		Draw() override;

	private:

		struct EntityNode
		{
			std::weak_ptr<game::Entity> _entity;
			bool						_collapsed = true;

			std::vector<EntityNode*>	_children;
			EntityNode*					_parent = nullptr;
		};

	private:

		void		OnNewEntityCallback(std::weak_ptr<game::Entity> entity);
		void		OnRemoveEntityCallback(std::weak_ptr<game::Entity> entity);
		void		OnRenameEntityCallback(std::weak_ptr<game::Entity> entity);

	private:

		std::weak_ptr<game::Entity>	_selection;

		EntityNode									_rootEntityNode;

		Event<std::weak_ptr<game::Entity>>::Slot	_onNewEntityCallback;
		Event<std::weak_ptr<game::Entity>>::Slot	_onRemoveEntityCallback;
		Event<std::weak_ptr<game::Entity>>::Slot	_onRenameEntityCallback;
	};
}
