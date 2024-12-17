#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/MaterialInstanceImporter.hpp"

namespace hod::editor
{
	///@brief 
	class HOD_EDITOR_API MaterialInstanceImporterCustomEditor : public ImporterCustomEditor
	{
	public:

												MaterialInstanceImporterCustomEditor() = default;
												MaterialInstanceImporterCustomEditor(const MaterialInstanceImporterCustomEditor&) = delete;
												MaterialInstanceImporterCustomEditor(MaterialInstanceImporterCustomEditor&&) = delete;
												~MaterialInstanceImporterCustomEditor() override = default;

		MaterialInstanceImporterCustomEditor	operator = (const MaterialInstanceImporterCustomEditor&) = delete;
		MaterialInstanceImporterCustomEditor	operator = (MaterialInstanceImporterCustomEditor&&) = delete;

	public:

		void									OnInit(std::shared_ptr<Asset> asset) override;
		bool									OnDrawInspector() override;

	private:

		MaterialInstanceAsset					_materialInstanceAsset;
		ReflectionDescriptor					_reflectionDescriptor;

		char									_paramsBuffer[4096];
	};
}
