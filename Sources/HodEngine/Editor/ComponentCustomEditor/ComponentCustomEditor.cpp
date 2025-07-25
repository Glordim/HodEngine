#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

#include "HodEngine/Editor/PropertyDrawer.hpp"

namespace hod::editor
{
	/// @brief 
	/// @param reflectedObject 
	/// @return 
	bool ComponentCustomEditor::OnDrawInspector(EditorReflectedObject &reflectedObject)
	{
		return PropertyDrawer::DrawDescriptor(reflectedObject);
	}

	/// @brief 
	/// @param component 
	/// @param projection 
	/// @param view 
	/// @param operation 
	/// @return 
	bool ComponentCustomEditor::OnDrawGizmo(game::Component* /*component*/, ViewportWindow& /*viewport*/, bool /*selected*/)
	{
		return false;
	}
}
