#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/CommandBuffer.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	class D3d12GraphicsPipeline;
	class D3d12PresentationSurface;
	class D3d12Texture;

	/// @brief
	class HOD_RHI_API D3d12CommandBuffer : public CommandBuffer
	{
	public:
		D3d12CommandBuffer();
		D3d12CommandBuffer(const D3d12CommandBuffer&) = delete;
		D3d12CommandBuffer(D3d12CommandBuffer&&) = delete;
		~D3d12CommandBuffer() override;

		void operator=(const D3d12CommandBuffer&) = delete;
		void operator=(D3d12CommandBuffer&&) = delete;

	public:
		ID3D12GraphicsCommandList* GetCommandList() const;

		bool StartRecord() override;
		bool EndRecord() override;

		bool StartRenderPass(RenderTarget* renderTarget = nullptr, PresentationSurface* presentationSurface = nullptr, const Color& color = Color(0.1f, 0.1f, 0.1f, 1.0f)) override;
		bool EndRenderPass() override;

		void SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) override;

		void SetViewport(const Rect& viewport) override;
		void SetScissor(const Rect& scissor) override;

		void SetGraphicsPipeline(const GraphicsPipeline* graphicsPipeline) override;
		void SetBindGroup(uint32_t set, const BindGroup* bindGroup, const uint32_t* uniformBufferOffsets, uint32_t uniformBufferOffsetCount) override;
		void SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset = 0) override;
		void SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) override;

		void Draw(uint32_t vertexCount) override;
		void DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;

		void Present(PresentationSurface* presentationSurface) override;

	private:
		void DrawEmulatedTriangleFan(uint32_t vertexCount);

	private:
		ComPtr<ID3D12CommandAllocator>    _commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> _commandList;

		const D3d12GraphicsPipeline* _graphicsPipeline = nullptr;

		// What the current render pass draws into
		D3d12Texture*             _renderTargetTexture = nullptr;
		D3d12PresentationSurface* _presentationSurface = nullptr;
	};
}
