#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API Node2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

										Node2dComponentCustomEditor();
										Node2dComponentCustomEditor(const Node2dComponentCustomEditor&) = delete;
										Node2dComponentCustomEditor(Node2dComponentCustomEditor&&) = delete;
										~Node2dComponentCustomEditor() override = default;

		Node2dComponentCustomEditor		operator = (const Node2dComponentCustomEditor&) = delete;
		Node2dComponentCustomEditor		operator = (Node2dComponentCustomEditor&&) = delete;

	public:

		bool							OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		Handle							_freeMoveHandle;

		Handle							_XAxisHandle;
		Handle							_YAxisHandle;

		Vector2							_initialPosition;
	};
}
