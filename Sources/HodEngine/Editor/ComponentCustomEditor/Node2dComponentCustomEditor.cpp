#include "HodEngine/Editor/Pch.hpp"
#include "Node2dComponentCustomEditor.hpp"

namespace hod::editor
{
	/// @brief 
	bool Node2dComponentCustomEditor::OnDrawInspector(void* object, ReflectionDescriptor* reflectionDescriptor)
	{
		return false;
	}

	/// @brief 
	bool Node2dComponentCustomEditor::OnDrawGizmo(void* object, ReflectionDescriptor* reflectionDescriptor)
	{
		return false;
	}
}
