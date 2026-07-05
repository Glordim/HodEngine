#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Math/Rect.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API MaterialImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(MaterialImporterSettings, ImporterSettings)

		Material::PolygonMode			_polygonMode = Material::PolygonMode::Fill;
		Material::Topololy			_topololy = Material::Topololy::TRIANGLE;

		Document								_defaultInstanceParams;
	};

	class HOD_EDITOR_API MaterialImporter : public Importer
	{
	public:

							MaterialImporter() = default;
							MaterialImporter(const MaterialImporter&) = delete;
							MaterialImporter(MaterialImporter&&) = delete;
							~MaterialImporter() override = default;

		MaterialImporter&	operator = (const MaterialImporter&) = delete;
		MaterialImporter&	operator = (MaterialImporter&&) = delete;

	public:

		Document&				GetDefaultInstanceParamsDocument();

	protected:

		bool	FillDataBlock(Stream& source, ImporterSettings* importSettings) override;
	};
}
