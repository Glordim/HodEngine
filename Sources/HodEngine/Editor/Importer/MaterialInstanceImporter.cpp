#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Importer/MaterialInstanceImporter.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"

#include <sstream>

namespace hod::editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialInstanceAsset, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialInstanceAsset::_material, "_material");
		AddPropertyT(reflectionDescriptor, &MaterialInstanceAsset::_params, "_params")->AddTrait<ReflectionTraitHide>();
	}

	DESCRIBE_REFLECTED_CLASS(MaterialInstanceImporterSettings, reflectionDescriptor)
	{

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
	bool MaterialInstanceImporter::WriteResource(FileSystem::Handle& data, FileSystem::Handle& meta, Document& document, Vector<Resource::Data>& datas, std::ofstream& thumbnail, ImporterSettings& settings)
	{
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
		return &renderer::MaterialInstanceResource::GetReflectionDescriptor();
	}

	std::shared_ptr<ImporterSettings> MaterialInstanceImporter::AllocateSettings() const
	{
		return std::make_shared<MaterialInstanceImporterSettings>();
	}
}
