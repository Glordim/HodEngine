#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceCooker.hpp"
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

	/// @brief 
	/// @param path 
	/// @return 
	bool MaterialInstanceCooker::FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages)
	{
		(void)asset;
		(void)platforms;
		(void)configs;
		(void)languages;
		/*
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, data) == false)
		{
			return false;
		}
		return true;
		*/
		return false;
	}
}
