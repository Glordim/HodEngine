#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"
#include <memory>

namespace hod::game
{
	class Entity;
	class Component;
}

namespace hod::editor
{
	class HierachyWindow;

	/// @brief 
	struct EntityDragAndDropPayload
	{
		HierachyWindow*					_hierarchyWindow;
		std::shared_ptr<game::Entity>	_entity;
	};

	/// @brief 
	class HOD_EDITOR_API HierachyWindow : public EditorTabWindow
	{
		META_TYPE(HierachyWindow, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					HierachyWindow() = default; // todo remove
					HierachyWindow(EditorTab* editorTab);
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

		bool										_openContextualMenu = false;

		EntityDragAndDropPayload					_entityDragAndDropPayload;

		//EntityNode									_rootEntityNode;
	};
}
