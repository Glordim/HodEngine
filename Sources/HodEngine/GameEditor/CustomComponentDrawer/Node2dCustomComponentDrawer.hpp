#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include <HodEngine/Math/Vector2.hpp>

#include "HodEngine/Editor/Gizmos/Gizmos.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API Node2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

										Node2dCustomComponentDrawer();
										Node2dCustomComponentDrawer(const Node2dCustomComponentDrawer&) = delete;
										Node2dCustomComponentDrawer(Node2dCustomComponentDrawer&&) = delete;
										~Node2dCustomComponentDrawer() override = default;

		Node2dCustomComponentDrawer		operator = (const Node2dCustomComponentDrawer&) = delete;
		Node2dCustomComponentDrawer		operator = (Node2dCustomComponentDrawer&&) = delete;

	public:

		bool							OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		Handle							_freeMoveHandle;

		Handle							_XAxisHandle;
		Handle							_YAxisHandle;

		math::Vector2							_initialPosition;
	};
}
