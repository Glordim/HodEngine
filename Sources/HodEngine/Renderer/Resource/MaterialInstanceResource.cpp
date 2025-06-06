#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod::renderer
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
	/// @param document 
	/// @param stream 
	/// @return 
	bool MaterialInstanceResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			// TODO message
			return false;
		}

		std::shared_ptr<MaterialResource> materialResource = _material.Lock();
		if (materialResource == nullptr)
		{
			return false;
		}
		Material* material = materialResource->GetMaterial();
		if (material == nullptr)
		{
			return false;
		}

		_materialInstance = Renderer::GetInstance()->CreateMaterialInstance(material);
		if (_materialInstance == nullptr)
		{
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
