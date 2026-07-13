#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

namespace hod::inline renderer
{
	DESCRIBE_REFLECTED_CLASS(MaterialInstanceResource, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &MaterialInstanceResource::_material, "_material");
		AddPropertyT(reflectionDescriptor, &MaterialInstanceResource::_params, "_params");
	}

	/// @brief 
	/// @return 
	MaterialInstanceResource::~MaterialInstanceResource()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
	}

	/// @brief
	/// @param resourceContainer
	/// @return
	bool MaterialInstanceResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* settingsDataBlock = resourceContainer.FindDataBlock("Settings");
		if (settingsDataBlock == nullptr)
		{
			OUTPUT_ERROR("MaterialInstanceResource::Initialize: missing 'Settings' data block");
			return false;
		}

		Document           settingsDocument;
		DocumentReaderJson documentReader;
		if (documentReader.Read(settingsDocument, *settingsDataBlock->_stream) == false)
		{
			OUTPUT_ERROR("MaterialInstanceResource::Initialize: can't read 'Settings' data block");
			return false;
		}

		if (Serializer::Deserialize(*this, settingsDocument.GetRootNode()) == false)
		{
			OUTPUT_ERROR("MaterialInstanceResource::Initialize: Unable to deserialize");
			return false;
		}

		std::shared_ptr<MaterialResource> materialResource = _material.Lock();
		if (materialResource == nullptr)
		{
			OUTPUT_ERROR("MaterialInstanceResource::Initialize: MaterialResource is nullptr");
			return false;
		}
		Material* material = materialResource->GetMaterial();
		if (material == nullptr)
		{
			OUTPUT_ERROR("MaterialInstanceResource::Initialize: Material is nullptr");
			return false;
		}

		_materialInstance = Renderer::GetInstance()->CreateMaterialInstance(material);
		if (_materialInstance == nullptr)
		{
			OUTPUT_ERROR("MaterialInstanceResource::Initialize: Unable to CreateMaterialInstance");
			return false;
		}

		MaterialSerializationHelper::ApplyParamsFromDocument(*_materialInstance, _params.GetRootNode(), _textureResources);

		return true;
	}

	/// @brief 
	/// @return 
	MaterialInstance* MaterialInstanceResource::GetMaterialInstance() const
	{
		return _materialInstance;
	}
}
