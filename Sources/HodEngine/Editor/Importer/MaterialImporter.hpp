#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API MaterialImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(MaterialImporterSettings, ImporterSettings)

		renderer::Material::PolygonMode			_polygonMode = renderer::Material::PolygonMode::Fill;
		renderer::Material::Topololy			_topololy = renderer::Material::Topololy::TRIANGLE;

		Document								_defaultInstanceParams;
	};

	class HOD_EDITOR_API MaterialImporter : public Importer
	{
	public:

							MaterialImporter();
							MaterialImporter(const MaterialImporter&) = delete;
							MaterialImporter(MaterialImporter&&) = delete;
							~MaterialImporter() override = default;

		MaterialImporter&	operator = (const MaterialImporter&) = delete;
		MaterialImporter&	operator = (MaterialImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;
		Document&				GetDefaultInstanceParamsDocument();

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
