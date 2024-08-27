#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"
#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::editor
{
	///@brief 
	ReflectionTraitComponentCustomEditor::ReflectionTraitComponentCustomEditor(ComponentCustomEditor* customEditor)
	: ReflectionTrait()
	, _customEditor(customEditor)
	{
	}

	/// @brief 
	ReflectionTraitComponentCustomEditor::~ReflectionTraitComponentCustomEditor()
	{
		delete _customEditor;
	}

	/// @brief 
	/// @return 
	ComponentCustomEditor* ReflectionTraitComponentCustomEditor::GetCustomEditor() const
	{
		return _customEditor;
	}
}
