#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

#include "HodEngine/Renderer/RHI/Vulkan/DescriptorSet.hpp"
#include "HodEngine/Renderer/RHI/Vulkan/BufferVk.hpp"

#include <vulkan/vulkan.h>

namespace hod::renderer
{
	class VkMaterial;

	/// @brief 
	class HOD_RENDERER_API VkMaterialInstance : public MaterialInstance
	{
	public:

										VkMaterialInstance(const Material& material);
										~VkMaterialInstance() override;

		std::vector<VkDescriptorSet>	GetDescriptorSets(uint32_t setOffset, uint32_t setCount) const;

	protected:

		void							ApplyInt(const std::string& memberName, int value) override;
		void							ApplyFloat(const std::string& memberName, float value) override;
		void							ApplyVec2(const std::string& memberName, const Vector2& value) override;
		void							ApplyVec4(const std::string& memberName, const Vector4& value) override;
		void							ApplyMat4(const std::string& memberName, const Matrix4& value) override;
		void							ApplyTexture(const std::string& name, const Texture& value) override;

	private:

		std::vector<DescriptorSet>		_descriptorSets;
	};
}
