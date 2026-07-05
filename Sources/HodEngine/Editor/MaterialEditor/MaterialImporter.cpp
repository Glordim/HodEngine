#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialImporter.hpp"

#include "HodEngine/Renderer/Resource/MaterialResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/ScopedGuard.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(MaterialImporterSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialImporterSettings::_polygonMode, "_polygonMode");
		AddPropertyT(reflectionDescriptor, &MaterialImporterSettings::_topololy, "_topololy");

		AddPropertyT(reflectionDescriptor, &MaterialImporterSettings::_defaultInstanceParams, "_defaultInstanceParams")->AddTrait<ReflectionTraitHide>();
	}

	bool MaterialImporter::FillDataBlock(Stream& source, ImporterSettings* importSettings)
	{
		(void)source;
		(void)importSettings;
		return false;
	}
}
