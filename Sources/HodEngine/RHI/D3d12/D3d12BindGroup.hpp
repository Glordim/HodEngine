#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/BindGroup.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"
#include "HodEngine/RHI/D3d12/D3d12DescriptorHeap.hpp"

namespace hod::inline rhi
{
	class Buffer;
	class D3d12GraphicsPipeline;

	/// @brief What D3d12CommandBuffer::SetBindGroup gives to the root parameters of a set (see D3d12GraphicsPipeline):
	/// the address of its uniform buffers, a table of the shader resource heap for its textures, and one of the sampler heap
	class HOD_RHI_API D3d12BindGroup : public BindGroup
	{
	public:
		D3d12BindGroup() = default;
		~D3d12BindGroup() override;

		bool Build(const D3d12GraphicsPipeline& graphicsPipeline, uint32_t set, Buffer* const* uniformBuffers, uint32_t uniformBufferCount, const TextureBinding* textureBindings,
		           uint32_t textureBindingCount);

		// One per uniform block of the set, in their order
		const Vector<D3D12_GPU_VIRTUAL_ADDRESS>& GetUniformBufferAddresses() const;

		D3D12_GPU_DESCRIPTOR_HANDLE GetTextureTable() const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerTable() const;

	private:
		Vector<D3D12_GPU_VIRTUAL_ADDRESS> _uniformBufferAddresses;

		uint32_t _textureTableIndex = D3d12DescriptorHeap::InvalidIndex; // in the shader resource heap, owned
		uint32_t _textureCount = 0;

		D3D12_GPU_DESCRIPTOR_HANDLE _samplerTable = {}; // shared, owned by the device
	};
}
