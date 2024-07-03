#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"

namespace hod::editor
{
	///@brief 
	class HOD_API TextureImporterCustomEditor : public ImporterCustomEditor
	{
	public:

									TextureImporterCustomEditor() = default;
									TextureImporterCustomEditor(const TextureImporterCustomEditor&) = delete;
									TextureImporterCustomEditor(ImporterCustomEditor&&) = delete;
									~TextureImporterCustomEditor() override = default;

		TextureImporterCustomEditor	operator = (const TextureImporterCustomEditor&) = delete;
		TextureImporterCustomEditor	operator = (TextureImporterCustomEditor&&) = delete;

	public:

		bool						OnDrawInspector(std::shared_ptr<Asset> asset) override;
	};
}
