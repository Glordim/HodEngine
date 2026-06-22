#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialInstanceAsset, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialInstanceAsset::_material, "_material");
		AddPropertyT(reflectionDescriptor, &MaterialInstanceAsset::_params, "_params")->AddTrait<ReflectionTraitHide>();
	}

	DESCRIBE_REFLECTED_CLASS(MaterialInstanceImporterSettings, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	MaterialInstanceImporter::MaterialInstanceImporter()
	: Importer()
	{
		SetSupportedDataFileExtensions("mati");
	}

	/// @brief 
	/// @param path 
	/// @return 
	bool MaterialInstanceImporter::WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail, ImporterSettings& settings)
	{
		// TODO
		(void)meta;
		(void)datas;
		(void)thumbnail;
		(void)settings;

		DocumentReaderJson documentReader;
		if (documentReader.Read(document, data) == false)
		{
			return false;
		}
		return true;
	}

	// TODO Move all virtual in Ctor const init ?

	/// @brief 
	/// @return 
	const char* MaterialInstanceImporter::GetTypeName() const
	{
		return "MaterialInstanceImporter";
	}

	ReflectionDescriptor* MaterialInstanceImporter::GetResourceDescriptor() const
	{
		return &MaterialInstanceResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> MaterialInstanceImporter::AllocateSettings() const
	{
		return std::make_shared<MaterialInstanceImporterSettings>();
	}
}
