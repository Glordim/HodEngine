#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API Node2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

										Node2dComponentCustomEditor();
										Node2dComponentCustomEditor(const Node2dComponentCustomEditor&) = delete;
										Node2dComponentCustomEditor(Node2dComponentCustomEditor&&) = delete;
										~Node2dComponentCustomEditor() override = default;

		Node2dComponentCustomEditor		operator = (const Node2dComponentCustomEditor&) = delete;
		Node2dComponentCustomEditor		operator = (Node2dComponentCustomEditor&&) = delete;

	public:

		bool							OnDrawGizmo(std::shared_ptr<game::Component> component, ViewportWindow& viewport) override;
		
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

		uint32_t						_movingAxis = 0;
	};
}
