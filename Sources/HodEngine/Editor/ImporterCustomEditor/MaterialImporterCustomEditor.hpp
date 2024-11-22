#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/MaterialImporter.hpp"

namespace hod::editor
{
	///@brief 
	class HOD_EDITOR_API MaterialImporterCustomEditor : public ImporterCustomEditor
	{
	public:

											MaterialImporterCustomEditor() = default;
											MaterialImporterCustomEditor(const MaterialImporterCustomEditor&) = delete;
											MaterialImporterCustomEditor(MaterialImporterCustomEditor&&) = delete;
											~MaterialImporterCustomEditor() override = default;

		MaterialImporterCustomEditor		operator = (const MaterialImporterCustomEditor&) = delete;
		MaterialImporterCustomEditor		operator = (MaterialImporterCustomEditor&&) = delete;

	public:

		void								OnInit(std::shared_ptr<Asset> asset) override;
		bool								OnDrawInspector() override;

	private:

		MaterialAsset						_materialAsset;
	};
}
