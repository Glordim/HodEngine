#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::editor
{
	/// @brief 
	bool ComponentCustomEditor::OnDrawInspector(void* object, ReflectionDescriptor* reflectionDescriptor)
	{
		return false;
	}

	/// @brief 
	bool ComponentCustomEditor::OnDrawGizmo(void* object, ReflectionDescriptor* reflectionDescriptor)
	{
		return false;
	}
}
