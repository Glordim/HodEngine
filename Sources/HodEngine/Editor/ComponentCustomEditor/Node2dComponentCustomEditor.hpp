#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::editor
{
	/// @brief 
	class HOD_API Node2dComponentCustomEditor : public ComponentCustomEditor
	{
	public:

										Node2dComponentCustomEditor() = default;
										Node2dComponentCustomEditor(const Node2dComponentCustomEditor&) = delete;
										Node2dComponentCustomEditor(Node2dComponentCustomEditor&&) = delete;
										~Node2dComponentCustomEditor() override = default;

		Node2dComponentCustomEditor		operator = (const Node2dComponentCustomEditor&) = delete;
		Node2dComponentCustomEditor		operator = (Node2dComponentCustomEditor&&) = delete;

	public:

		bool							OnDrawInspector(void* object, ReflectionDescriptor* reflectionDescriptor) override;
		bool							OnDrawGizmo(void* object, ReflectionDescriptor* reflectionDescriptor) override;
	};
}
