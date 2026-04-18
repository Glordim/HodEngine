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

		Vector<VkDescriptorSet>	GetDescriptorSets(uint32_t setOffset, uint32_t setCount) const;

	protected:

		void							ApplyInt(const String& memberName, int value) override;
		void							ApplyFloat(const String& memberName, float value) override;
		void							ApplyVec2(const String& memberName, const Vector2& value) override;
		void							ApplyVec4(const String& memberName, const Vector4& value) override;
		void							ApplyMat4(const String& memberName, const Matrix4& value) override;
		void							ApplyTexture(const String& name, const Texture& value) override;

	private:

		Vector<DescriptorSet>		_descriptorSets;
	};
}
