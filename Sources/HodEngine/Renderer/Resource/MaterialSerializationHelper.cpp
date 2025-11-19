#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Resource/WeakResource.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"

namespace hod::renderer
{
	/// @brief
	/// @param material
	/// @param params
	void MaterialSerializationHelper::GenerateParameters(const Material& material, Vector<ShaderParameter>& params)
	{
		const std::unordered_map<uint32_t, ShaderSetDescriptor*>& setDescriptors = material.GetSetDescriptors();

		for (const auto& pair : setDescriptors)
		{
			for (const ShaderSetDescriptor::BlockUbo& ubo : pair.second->GetUboBlocks())
			{
				for (const auto& childPair : ubo._rootMember._childsMap)
				{
					if (childPair.second._memberType == ShaderSetDescriptor::BlockUbo::MemberType::Float)
					{
						params.EmplaceBack(childPair.second._name, ShaderParameter::Type::Float);
					}
					else if (childPair.second._memberType == ShaderSetDescriptor::BlockUbo::MemberType::Float2)
					{
						params.EmplaceBack(childPair.second._name, ShaderParameter::Type::Float2);
					}
					else if (childPair.second._memberType == ShaderSetDescriptor::BlockUbo::MemberType::Float4)
					{
						params.EmplaceBack(childPair.second._name, ShaderParameter::Type::Float4);
					}
				}
			}

			for (const ShaderSetDescriptor::BlockTexture& texture : pair.second->GetTextureBlocks())
			{
				if (texture._type == ShaderSetDescriptor::BlockTexture::Texture)
				{
					params.EmplaceBack(texture._name, ShaderParameter::Type::Texture);
				}
			}
		}
	}

	/// @brief
	void MaterialSerializationHelper::ApplyParamsFromDocument(MaterialInstance& materialInstance, const Document::Node& paramsNode,
	                                                          Vector<WeakResource<TextureResource>>& textureResources)
	{
		const Document::Node* paramNode = paramsNode.GetFirstChild();
		while (paramNode != nullptr)
		{
			String                name = paramNode->GetChild("Name")->GetString();
			ShaderParameter::Type type = static_cast<ShaderParameter::Type>(paramNode->GetChild("Type")->GetUInt8());
			const Document::Node* valueNode = paramNode->GetChild("Value");
			switch (type)
			{
				case ShaderParameter::Type::Float:
				{
					float value = valueNode->GetFloat32();
					materialInstance.SetFloat("ubo." + name, value);
				}
				break;

				case ShaderParameter::Type::Float2:
				{
					Vector2 value;
					Serializer::Deserialize(value, *valueNode);
					materialInstance.SetVec2("ubo." + name, value);
				}
				break;

				case ShaderParameter::Type::Float4:
				{
					Vector4 value;
					Serializer::Deserialize(value, *valueNode);
					materialInstance.SetVec4("ubo." + name, value);
				}
				break;

				case ShaderParameter::Type::Texture:
				{
					WeakResource<TextureResource> value;
					Serializer::Deserialize(value, *valueNode);
					std::shared_ptr<TextureResource> textureResource = value.Lock();
					if (textureResource != nullptr)
					{
						materialInstance.SetTexture(name, textureResource->GetTexture());
					}

					textureResources.push_back(value);
				}
				break;

				default: break;
			}
			paramNode = paramNode->GetNextSibling();
		}

		/*
		const ReflectionDescriptor& reflectionDescriptor = const_cast<Material&>(materialInstance.GetMaterial()).GetReflectionDescriptorForParameters();

		char paramsBuffer[4096];
		Serializer::Deserialize(reflectionDescriptor, static_cast<void*>(paramsBuffer), paramsNode);
		MaterialSerializationHelper::ApplyReflectedParams(materialInstance, reflectionDescriptor, paramsBuffer, "", textureResources);
		*/
	}

	/// @brief
	/// @param reflectionDescriptor
	/// @param instance
	/// @param path
	void MaterialSerializationHelper::ApplyReflectedParams(MaterialInstance& materialInstance, const ReflectionDescriptor& reflectionDescriptor, void* instance, String path,
	                                                       Vector<WeakResource<TextureResource>>& textureResources)
	{
		for (ReflectionProperty* property : reflectionDescriptor.GetProperties())
		{
			String newPath;
			if (path.Empty())
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
						materialInstance.SetFloat(newPath, value);
					}
					break;

					default:
					{
						OUTPUT_ERROR("MaterialInstance::ApplyReflectedParams: Unsupported variable type");
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
					materialInstance.SetVec2(newPath, *value);
				}
				else if (propertyObject->GetReflectionDescriptor() == &Vector4::GetReflectionDescriptor())
				{
					Vector4* value = static_cast<Vector4*>(propertyObject->GetInstance(instance));
					materialInstance.SetVec4(newPath, *value);
				}
				else if (propertyObject->GetReflectionDescriptor() == &WeakResource<TextureResource>::GetReflectionDescriptor())
				{
					WeakResource<TextureResource>* value = static_cast<WeakResource<TextureResource>*>(propertyObject->GetInstance(instance));
					WeakResource<TextureResource>  retain;
					retain.SetUid(value->GetUid());

					std::shared_ptr<TextureResource> textureResource = retain.Lock();
					if (textureResource != nullptr)
					{
						textureResources.push_back(retain);
						materialInstance.SetTexture(newPath, textureResource->GetTexture());
					}
					else
					{
						OUTPUT_ERROR("MaterialInstance::ApplyReflectedParams: TextureResource not found");
					}
				}
				else
				{
					ApplyReflectedParams(materialInstance, *propertyObject->GetReflectionDescriptor(), propertyObject->GetInstance(instance), newPath, textureResources);
				}
			}
			else
			{
				OUTPUT_ERROR("MaterialInstance::ApplyReflectedParams: Unsupported property type");
			}
		}
	}
}
