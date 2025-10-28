#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/ShaderSetDescriptorVk.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterial.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkMaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp"

namespace hod::renderer
{
	/// @brief
	/// @param material
	VkMaterialInstance::VkMaterialInstance(const Material& material)
	: MaterialInstance(material)
	{
		const VkMaterial*                                         vkMaterial = static_cast<const VkMaterial*>(&material);
		const std::unordered_map<uint32_t, ShaderSetDescriptor*>& descriptorSetLayoutMap = vkMaterial->GetSetDescriptors();

		size_t descriptorSetLayoutCount = descriptorSetLayoutMap.size();

		_descriptorSets.Resize(descriptorSetLayoutCount);

		auto it = descriptorSetLayoutMap.cbegin();
		auto itEnd = descriptorSetLayoutMap.cend();
		int  i = 0;

		while (it != itEnd)
		{
			DescriptorSet& descriptorSet = _descriptorSets[i];

			descriptorSet.SetLayout(static_cast<ShaderSetDescriptorVk*>(it->second));

			++i;
			++it;
		}
	}

	/// @brief
	VkMaterialInstance::~VkMaterialInstance() {}

	/// @brief
	/// @param setOffset
	/// @param setCount
	/// @return
	Vector<VkDescriptorSet> VkMaterialInstance::GetDescriptorSets(uint32_t setOffset, uint32_t setCount) const
	{
		if (setOffset > _descriptorSets.Size())
		{
			return Vector<VkDescriptorSet>();
		}

		if (setCount > _descriptorSets.Size() - setOffset)
		{
			setCount = (uint32_t)_descriptorSets.Size() - setOffset;
		}

		Vector<VkDescriptorSet> descriptorSets(setCount);

		for (size_t i = 0; i < setCount; ++i)
		{
			descriptorSets[i] = _descriptorSets[setOffset + i].GetDescriptorSet();
		}

		return descriptorSets;
	}

	/// @brief
	/// @param memberName
	/// @param value
	void VkMaterialInstance::ApplyInt(const String& memberName, int value)
	{
		size_t descriptorSetCount = _descriptorSets.Size();
		for (size_t i = 0; i < descriptorSetCount; ++i)
		{
			_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
		}
	}

	/// @brief
	/// @param memberName
	/// @param value
	void VkMaterialInstance::ApplyFloat(const String& memberName, float value)
	{
		size_t descriptorSetCount = _descriptorSets.Size();
		for (size_t i = 0; i < descriptorSetCount; ++i)
		{
			_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
		}
	}

	/// @brief
	/// @param memberName
	/// @param value
	void VkMaterialInstance::ApplyVec2(const String& memberName, const Vector2& value)
	{
		size_t descriptorSetCount = _descriptorSets.Size();
		for (size_t i = 0; i < descriptorSetCount; ++i)
		{
			_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
		}
	}

	/// @brief
	/// @param memberName
	/// @param value
	void VkMaterialInstance::ApplyVec4(const String& memberName, const Vector4& value)
	{
		size_t descriptorSetCount = _descriptorSets.Size();
		for (size_t i = 0; i < descriptorSetCount; ++i)
		{
			_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
		}
	}

	/// @brief
	/// @param memberName
	/// @param value
	void VkMaterialInstance::ApplyMat4(const String& memberName, const Matrix4& value)
	{
		size_t descriptorSetCount = _descriptorSets.Size();
		for (size_t i = 0; i < descriptorSetCount; ++i)
		{
			_descriptorSets[i].SetUboValue(memberName, &value, sizeof(value));
		}
	}

	/// @brief
	/// @param name
	/// @param value
	void VkMaterialInstance::ApplyTexture(const String& name, const Texture& value)
	{
		size_t descriptorSetCount = _descriptorSets.Size();
		for (size_t i = 0; i < descriptorSetCount; ++i)
		{
			_descriptorSets[i].SetTexture(name, (VkTexture*)&value);
		}
	}
}
