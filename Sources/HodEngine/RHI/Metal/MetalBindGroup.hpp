#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/BindGroup.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline rhi
{
	class Buffer;
	class MetalBuffer;
	class MetalGraphicsPipeline;

	/// @brief On Metal the shaders read a set from an argument buffer: a struct holding the address of each of its uniform buffers
	/// and the id of each of its textures and samplers, at the offset the reflection gives them (the binding of their block).
	/// The address of a uniform buffer includes where it is read, which changes from one draw to the next: the BindGroup only keeps
	/// what the argument buffer is made of, MetalCommandBuffer::SetBindGroup writes one each time the BindGroup is bound.
	class HOD_RHI_API MetalBindGroup : public BindGroup
	{
	public:
		MetalBindGroup() = default;
		~MetalBindGroup() override = default;

		bool Build(const MetalGraphicsPipeline& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount, const TextureBinding* textureBindings,
		           uint32_t textureBindingCount);

		// Index of the buffer each stage reads the argument buffer of the set from: they can differ, the stages are compiled apart.
		// ShaderSetDescriptor::NoSpace for a stage whose shader does not use the set.
		uint32_t GetVertexBufferIndex() const;
		uint32_t GetFragmentBufferIndex() const;

		uint32_t GetArgumentBufferSize() const;
		uint32_t GetUniformBufferCount() const;

		// argumentBuffer: GetArgumentBufferSize bytes. uniformBufferOffsets: one per uniform block of the set, in their order
		void WriteArgumentBuffer(void* argumentBuffer, const uint32_t* uniformBufferOffsets) const;

	private:
		struct UniformBuffer
		{
			const MetalBuffer* _buffer = nullptr;
			uint32_t           _offset = 0; // of its address in the argument buffer
		};

	private:
		uint32_t              _vertexBufferIndex = 0;
		uint32_t              _fragmentBufferIndex = 0;
		Vector<uint8_t>       _argumentBuffer; // textures and samplers are in, the addresses of the uniform buffers are left to WriteArgumentBuffer
		Vector<UniformBuffer> _uniformBuffers;
	};
}
