#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"

namespace hod::editor
{
	///@brief 
	class HOD_EDITOR_API SerializedDataImporterCustomEditor : public ImporterCustomEditor
	{
	public:

											SerializedDataImporterCustomEditor() = default;
											SerializedDataImporterCustomEditor(const SerializedDataImporterCustomEditor&) = delete;
											SerializedDataImporterCustomEditor(SerializedDataImporterCustomEditor&&) = delete;
											~SerializedDataImporterCustomEditor() override = default;

		SerializedDataImporterCustomEditor	operator = (const SerializedDataImporterCustomEditor&) = delete;
		SerializedDataImporterCustomEditor	operator = (SerializedDataImporterCustomEditor&&) = delete;

	public:

		void								OnInit(std::shared_ptr<Asset> asset) override;
		bool								OnDrawInspector() override;

	private:

		SerializedDataAsset					_serializedDataAsset;
	};
}
