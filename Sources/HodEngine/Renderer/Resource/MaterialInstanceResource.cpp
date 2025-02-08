#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"
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
		delete _materialInstance;
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

		char paramsBuffer[4096];
		Serializer::Deserialize(material->GetReflectionDescriptorForParameters(), static_cast<void*>(paramsBuffer), _params.GetRootNode());
		ApplyInitialParams(&material->GetReflectionDescriptorForParameters(), static_cast<void*>(paramsBuffer), "");

		return true;
	}

	/// @brief 
	/// @param reflectionDescriptor 
	/// @param instance 
	/// @param path 
	void MaterialInstanceResource::ApplyInitialParams(const ReflectionDescriptor* reflectionDescriptor, void* instance, std::string path)
	{
		for (ReflectionProperty* property : reflectionDescriptor->GetProperties())
		{
			std::string newPath;
			if (path.empty())
			{
				newPath = property->GetName();
			}
			else
			{
				newPath = path + "." + property->GetName();
			}

			if (property->GetMetaType() == ReflectionPropertyVariable::GetMetaTypeStatic())
			{
				ReflectionPropertyVariable* propertyVariable = static_cast<ReflectionPropertyVariable*>(property);
				switch (propertyVariable->GetType())
				{
					case ReflectionPropertyVariable::Type::Float32:
					{
						float value = propertyVariable->GetValue<float>(instance);
						_materialInstance->SetFloat(newPath, value);
					}
					break;

					default:
					{
						OUTPUT_ERROR("MaterialInstanceResource::ApplyInitialParams: Unsupported variable type");
					}
					break;
				}
			}
			else if (property->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
			{
				ReflectionPropertyObject* propertyObject = static_cast<ReflectionPropertyObject*>(property);

				if (propertyObject->GetReflectionDescriptor() == &Vector2::GetReflectionDescriptor())
				{
					Vector2* value = static_cast<Vector2*>(propertyObject->GetInstance(instance));
					_materialInstance->SetVec2(newPath, *value);
				}
				else if (propertyObject->GetReflectionDescriptor() == &Vector4::GetReflectionDescriptor())
				{
					Vector4* value = static_cast<Vector4*>(propertyObject->GetInstance(instance));
					_materialInstance->SetVec4(newPath, *value);
				}
				else if (propertyObject->GetReflectionDescriptor() == &WeakResource<TextureResource>::GetReflectionDescriptor())
				{
					WeakResource<TextureResource>* value = static_cast<WeakResource<TextureResource>*>(propertyObject->GetInstance(instance));
					WeakResource<TextureResource> retain;
					retain.SetUid(value->GetUid());

					std::shared_ptr<TextureResource> textureResource = retain.Lock();
					if (textureResource != nullptr)
					{
						_texturesRetain.push_back(retain);
						_materialInstance->SetTexture(newPath, textureResource->GetTexture());
					}
					else
					{
						OUTPUT_ERROR("MaterialInstanceResource::ApplyInitialParams: TextureResource not found");
					}
				}
				else
				{
					ApplyInitialParams(propertyObject->GetReflectionDescriptor(), propertyObject->GetInstance(instance), newPath);
				}
			}
			else
			{
				OUTPUT_ERROR("MaterialInstanceResource::ApplyInitialParams: Unsupported property type");
			}
		}
	}

	/// @brief 
	/// @return 
	MaterialInstance* MaterialInstanceResource::GetMaterialInstance() const
	{
		return _materialInstance;
	}
}
