#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTabWindow.hpp"
#include <memory>

namespace hod::inline game
{
	class Entity;
	class Component;
}

namespace hod::inline editor
{
	class HierachyWindow;

	/// @brief 
	struct EntityDragAndDropPayload
	{
		HierachyWindow*	_hierarchyWindow;
		Entity*	_entity;
	};

	/// @brief 
	class HOD_EDITOR_API HierachyWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(HierachyWindow, EditorTabWindow)

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
			std::weak_ptr<Entity> _entity;
			bool						_collapsed = true;

			Vector<EntityNode*>	_children;
			EntityNode*					_parent = nullptr;
		};
		*/

	private:

		//void		DrawEntityNode(EntityNode* entityNode);
		void		DrawEntity(Entity* entity);

		void		OnAddEntityCallback(Entity* entity);
		void		OnRemoveEntityCallback(Entity* entity);
		void		OnRenameEntityCallback(Entity* entity);

		void		OnAddComponentCallback(Component* componenent);
		void		OnRemoveComponentCallback(Component* componenent);

	private:

		bool										_openContextualMenu = false;

		EntityDragAndDropPayload					_entityDragAndDropPayload;

		Entity*								_previousSelection = nullptr;

		//EntityNode									_rootEntityNode;
	};
}
