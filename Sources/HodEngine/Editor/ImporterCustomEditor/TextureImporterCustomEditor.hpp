#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"

namespace hod::renderer
{
	class TextureResource;
}

namespace hod::editor
{
	///@brief 
	class HOD_EDITOR_API TextureImporterCustomEditor : public ImporterCustomEditor
	{
	public:

									TextureImporterCustomEditor() = default;
									TextureImporterCustomEditor(const TextureImporterCustomEditor&) = delete;
									TextureImporterCustomEditor(ImporterCustomEditor&&) = delete;
									~TextureImporterCustomEditor() override = default;

		TextureImporterCustomEditor	operator = (const TextureImporterCustomEditor&) = delete;
		TextureImporterCustomEditor	operator = (TextureImporterCustomEditor&&) = delete;

	public:

		void						OnInit(std::shared_ptr<Asset> asset) override;
		bool						OnDrawInspector() override;

	private:

		std::shared_ptr<renderer::TextureResource>	_texture;
	};
}
