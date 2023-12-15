#if defined(HOD_EDITOR)

#include "HodEngine/Game/Traits/ReflectionTraitCustomEditor.hpp"
#include "HodEngine/Game/Traits/ComponentCustomEditor.hpp"

namespace hod
{
	///@brief 
	ReflectionTraitCustomEditor::ReflectionTraitCustomEditor(ComponentCustomEditor* customEditor)
	: ReflectionTrait()
	, _customEditor(customEditor)
	{
	}

	/// @brief 
	ReflectionTraitCustomEditor::~ReflectionTraitCustomEditor()
	{
		delete _customEditor;
	}

	/// @brief 
	/// @return 
	ComponentCustomEditor* ReflectionTraitCustomEditor::GetCustomEditor() const
	{
		return _customEditor;
	}
}

#endif
