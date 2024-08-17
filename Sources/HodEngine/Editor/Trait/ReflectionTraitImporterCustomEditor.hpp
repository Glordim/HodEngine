#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

namespace hod::editor
{
	class ImporterCustomEditor;

	///@brief 
	class HOD_EDITOR_API ReflectionTraitImporterCustomEditor : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitImporterCustomEditor, ReflectionTrait)

	public:

												ReflectionTraitImporterCustomEditor(ImporterCustomEditor* customEditor);
												ReflectionTraitImporterCustomEditor(const ReflectionTraitImporterCustomEditor& copy) = default;
												ReflectionTraitImporterCustomEditor(ReflectionTraitImporterCustomEditor&& move) = default;
												~ReflectionTraitImporterCustomEditor() override;

		ReflectionTraitImporterCustomEditor&	operator = (const ReflectionTraitImporterCustomEditor& copy) = default;
		ReflectionTraitImporterCustomEditor&	operator = (ReflectionTraitImporterCustomEditor&& move) = default;

	public:

		ImporterCustomEditor*			GetCustomEditor() const;

	private:

		ImporterCustomEditor*			_customEditor = nullptr;
	};
}
