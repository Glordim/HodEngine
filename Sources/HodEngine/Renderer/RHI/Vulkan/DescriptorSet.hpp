#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vulkan/vulkan.h>

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

namespace hod::inline renderer
{
	class ShaderSetDescriptorVk;
	class VkTexture;
	class BufferVk;

	/// @brief
	class HOD_RENDERER_API DescriptorSet
	{
	public:
		DescriptorSet();
		virtual ~DescriptorSet();

		bool SetLayout(const ShaderSetDescriptorVk* layout);

		VkDescriptorSet GetDescriptorSet();

		void SetUboValue(const String& memberName, const void* value, size_t valueSize);
		void SetTexture(const String& name, const VkTexture* textureSampler);

	private:
		void FlushUboUpdates(uint32_t frameIndex);
		void FlushTextureUpdates(uint32_t frameIndex);

		const ShaderSetDescriptorVk* _descriptorSetLayout = nullptr;

		struct PerFrameData
		{
			VkDescriptorSet   descriptorSet = VK_NULL_HANDLE;
			Vector<BufferVk*> uboBuffers;
			bool              uboDirty     = false;
			bool              textureDirty = false;
		};
		Vector<PerFrameData> _perFrameData;

		// CPU-side mirror of each UBO block — ground truth, flushed lazily to each FIF buffer
		Vector<Vector<uint8_t>> _uboData;

		// Desired texture state per texture block index, flushed lazily to each FIF descriptor set
		Vector<const VkTexture*> _currentTextures;
	};
}
