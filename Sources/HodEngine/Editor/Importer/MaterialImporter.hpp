#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"

#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/ShaderResource.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API MaterialAsset
	{
		REFLECTED_CLASS_NO_PARENT(MaterialAsset)

		virtual ~MaterialAsset() = default;

		WeakResource<renderer::ShaderResource>	_vertexShader;
		WeakResource<renderer::ShaderResource>	_fragmentShader;
		
		renderer::Material::PolygonMode		_polygonMode = renderer::Material::PolygonMode::Fill;
		renderer::Material::Topololy		_topololy = renderer::Material::Topololy::TRIANGLE;

		Document								_defaultInstanceParams;
	};

	class HOD_EDITOR_API MaterialImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(MaterialImporterSettings, ImporterSettings)
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

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
