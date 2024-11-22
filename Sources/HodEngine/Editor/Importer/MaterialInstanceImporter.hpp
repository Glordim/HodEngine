#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Rect.hpp"

#include "HodEngine/Core/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

namespace hod::editor
{
	class HOD_EDITOR_API MaterialInstanceAsset
	{
		REFLECTED_CLASS_NO_PARENT(MaterialInstanceAsset)

		WeakResource<renderer::MaterialResource>	_material;
	};

	class HOD_EDITOR_API MaterialInstanceImporterSettings : public ImporterSettings
	{
		REFLECTED_CLASS(MaterialInstanceImporterSettings, ImporterSettings)
	};

	class HOD_EDITOR_API MaterialInstanceImporter : public Importer
	{
	public:

									MaterialInstanceImporter();
									MaterialInstanceImporter(const MaterialInstanceImporter&) = delete;
									MaterialInstanceImporter(MaterialInstanceImporter&&) = delete;
									~MaterialInstanceImporter() override = default;

		MaterialInstanceImporter&	operator = (const MaterialInstanceImporter&) = delete;
		MaterialInstanceImporter&	operator = (MaterialInstanceImporter&&) = delete;

	public:

		std::shared_ptr<ImporterSettings> AllocateSettings() const override;
		const char*				GetTypeName() const override;
		ReflectionDescriptor*	GetResourceDescriptor() const override;

	protected:

		bool				WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) override;
	};
}
