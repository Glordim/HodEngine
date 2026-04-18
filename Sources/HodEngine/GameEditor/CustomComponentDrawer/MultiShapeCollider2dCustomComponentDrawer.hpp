#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API MultiShapeCollider2dCustomComponentDrawer : public CustomComponentDrawer
	{
	public:

													MultiShapeCollider2dCustomComponentDrawer();
													MultiShapeCollider2dCustomComponentDrawer(const MultiShapeCollider2dCustomComponentDrawer&) = delete;
													MultiShapeCollider2dCustomComponentDrawer(MultiShapeCollider2dCustomComponentDrawer&&) = delete;
													~MultiShapeCollider2dCustomComponentDrawer() override;

		MultiShapeCollider2dCustomComponentDrawer	operator = (const MultiShapeCollider2dCustomComponentDrawer&) = delete;
		MultiShapeCollider2dCustomComponentDrawer	operator = (MultiShapeCollider2dCustomComponentDrawer&&) = delete;

	public:

		bool										OnDrawGizmo(Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*					_materialInstance = nullptr;
	};
}
