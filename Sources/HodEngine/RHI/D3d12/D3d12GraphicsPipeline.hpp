#pragma once
#include "HodEngine/RHI/Export.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	/// @brief A pipeline state and its root signature, which is laid out as:
	/// - the push constant, as root constants
	/// - then for each set: one root constant buffer view per uniform block (where it is read is given with each SetBindGroup,
	///   like the dynamic offsets of Vulkan), one descriptor table for its textures and one for its samplers
	class HOD_RHI_API D3d12GraphicsPipeline : public GraphicsPipeline
	{
	public:
		static constexpr uint32_t NoRootParameter = 0xFFFFFFFF;

		/// @brief Where the resources of a set go in the root signature
		struct SetLayout
		{
			uint32_t _firstUniformBufferRootParameter = NoRootParameter; // followed by the ones of the next uniform blocks of the set
			uint32_t _textureTableRootParameter = NoRootParameter;
			uint32_t _samplerTableRootParameter = NoRootParameter;

			// Size of the tables. The textures, and the samplers, are in the order of their blocks in the set.
			uint32_t _textureCount = 0;
			uint32_t _samplerCount = 0;
		};

	public:
		D3d12GraphicsPipeline();
		~D3d12GraphicsPipeline() override;

		bool Build(const CreateInfo& createInfo) override;

		ID3D12PipelineState* GetPipelineState() const;
		ID3D12RootSignature* GetRootSignature() const;

		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;
		// The topology is a triangle list: fans have to be drawn as such, see D3d12CommandBuffer::Draw
		bool IsTriangleFanEmulated() const;

		// nullptr if the pipeline has no such set
		const SetLayout* GetSetLayout(uint32_t set) const;

		uint32_t GetPushConstantRootParameter() const;
		uint32_t GetPushConstantSize() const;

		uint32_t GetVertexStride(uint32_t binding) const;

	private:
		bool BuildRootSignature(Shader* vertexShader, Shader* fragmentShader);

	private:
		ComPtr<ID3D12PipelineState> _pipelineState;
		ComPtr<ID3D12RootSignature> _rootSignature;

		D3D_PRIMITIVE_TOPOLOGY _primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		bool                   _triangleFanEmulated = false;

		Vector<SetLayout> _setLayouts; // indexed by set

		uint32_t _pushConstantRootParameter = NoRootParameter;
		uint32_t _pushConstantSize = 0;

		Vector<uint32_t> _vertexStrides; // indexed by binding
	};
}
