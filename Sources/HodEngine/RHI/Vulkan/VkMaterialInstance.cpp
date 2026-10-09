#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"
#include "HodEngine/RHI/Vulkan/GraphicsPipelineVulkan.hpp"
#include "HodEngine/RHI/Vulkan/VkMaterialInstance.hpp"
#include "HodEngine/RHI/Vulkan/VkShader.hpp"

#include "HodEngine/RHI/Vulkan/RhiDeviceVulkan.hpp"

namespace hod::inline rhi
{
	/// @brief
	/// @param graphicsPipeline
	VkMaterialInstance::VkMaterialInstance(const GraphicsPipeline& graphicsPipeline)
	: MaterialInstance(graphicsPipeline)
	{
		const GraphicsPipelineVulkan*                                         graphicsPipelineVulkan = static_cast<const GraphicsPipelineVulkan*>(&graphicsPipeline);
		const std::map<uint32_t, ShaderSetDescriptor*>& descriptorSetLayoutMap = graphicsPipelineVulkan->GetSetDescriptors();

		// The sets of a pipeline are contiguous from 0, so _descriptorSets is indexed by set
		_descriptorSets.Resize(descriptorSetLayoutMap.size());

		for (const auto& pair : descriptorSetLayoutMap)
		{
			_descriptorSets[pair.first].SetLayout(pair.second, graphicsPipelineVulkan->GetDescriptorSetLayout(pair.first));
		}
	}

	/// @brief
	VkMaterialInstance::~VkMaterialInstance() {}

	/// @brief
	/// @param setOffset
	/// @param setCount
	/// @return
	Vector<VkDescriptorSet> VkMaterialInstance::GetDescriptorSets(uint32_t setOffset, uint32_t setCount)
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
