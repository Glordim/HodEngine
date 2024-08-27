#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Trait/ReflectionTraitImporterCustomEditor.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"

namespace hod::editor
{
	///@brief 
	ReflectionTraitImporterCustomEditor::ReflectionTraitImporterCustomEditor(ImporterCustomEditor* customEditor)
	: ReflectionTrait()
	, _customEditor(customEditor)
	{
	}

	/// @brief 
	ReflectionTraitImporterCustomEditor::~ReflectionTraitImporterCustomEditor()
	{
		delete _customEditor;
	}

	/// @brief 
	/// @return 
	ImporterCustomEditor* ReflectionTraitImporterCustomEditor::GetCustomEditor() const
	{
		return _customEditor;
	}
}
