#pragma once

#if defined(HOD_EDITOR)

#include "HodEngine/Game/Traits/ComponentCustomEditor.hpp"

namespace hod
{
	namespace game
	{
		/// @brief 
		class Node2dComponentCustomEditor : public ComponentCustomEditor
		{
		public:

											Node2dComponentCustomEditor() = default;
											Node2dComponentCustomEditor(const Node2dComponentCustomEditor&) = delete;
											Node2dComponentCustomEditor(Node2dComponentCustomEditor&&) = delete;
											~Node2dComponentCustomEditor() override = default;

			Node2dComponentCustomEditor		operator = (const Node2dComponentCustomEditor&) = delete;
			Node2dComponentCustomEditor		operator = (Node2dComponentCustomEditor&&) = delete;

		public:

			void							OnDrawInspector() override;
			void							OnDrawGizmo() override;
		};
	}
}

#endif
