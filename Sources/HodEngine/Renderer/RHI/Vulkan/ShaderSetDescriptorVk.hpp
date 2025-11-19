#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Renderer/RHI/ShaderSetDescriptor.hpp"

#include <vulkan/vulkan.h>

namespace spirv_cross
{
	class Compiler;
	struct Resource;
	struct SPIRType;
}

namespace hod::renderer
{
	/// @brief 
	class HOD_RENDERER_API ShaderSetDescriptorVk : public ShaderSetDescriptor
	{
	public:

		static VkDescriptorType TextureTypeToVkDescriptorType(BlockTexture::Type type);

	public:

											ShaderSetDescriptorVk();
											~ShaderSetDescriptorVk() override;

		VkDescriptorSetLayout				GetDescriptorSetLayout() const;

		void								ExtractBlockUbo(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource);
		void								ExtractUboSubMembers(const spirv_cross::Compiler& comp, const spirv_cross::SPIRType& type, BlockUbo::Member& member);

		void								ExtractBlockTexture(const spirv_cross::Compiler& comp, const spirv_cross::Resource& resource, VkDescriptorType type);

		bool								BuildDescriptorSetLayout();

	private:

		VkDescriptorSetLayout				_descriptorSetLayout = VK_NULL_HANDLE;
	};
}
