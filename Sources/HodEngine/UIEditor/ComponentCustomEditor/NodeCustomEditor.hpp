#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"
#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_UI_EDITOR_API NodeCustomEditor : public ComponentCustomEditor
	{
	public:

										NodeCustomEditor();
										NodeCustomEditor(const NodeCustomEditor&) = delete;
										NodeCustomEditor(NodeCustomEditor&&) = delete;
										~NodeCustomEditor() override;

		NodeCustomEditor				operator = (const NodeCustomEditor&) = delete;
		NodeCustomEditor				operator = (NodeCustomEditor&&) = delete;

	public:

		bool							OnDrawGizmo(game::Component* component, ViewportWindow& viewport) override;

	private:

		static Vector2					GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport);
		
	private:

		renderer::MaterialInstance*		_materialInstanceCenterNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceCenterHightlight = nullptr;

		renderer::MaterialInstance*		_materialInstanceAxisXNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceAxisXHightlight = nullptr;
		uint32_t						_pickingIdAxisX;

		renderer::MaterialInstance*		_materialInstanceAxisYNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceAxisYHightlight = nullptr;
		uint32_t						_pickingIdAxisY;

		renderer::MaterialInstance*		_materialInstanceAxisZNormal = nullptr;
		renderer::MaterialInstance*		_materialInstanceAxisZHightlight = nullptr;
		uint32_t						_pickingIdAxisZ;

		renderer::MaterialInstance*		_materialInstance = nullptr;

		uint32_t						_movingAxis = 0;
		Vector2							_pickingOffset;
	};
}
