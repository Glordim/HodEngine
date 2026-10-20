#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Texture.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"
#include "HodEngine/RHI/D3d12/D3d12DescriptorHeap.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API D3d12Texture : public Texture
	{
	public:
		// The state a texture is in whenever it is not being drawn into, or copied
		static constexpr D3D12_RESOURCE_STATES ShaderResourceState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

		static DXGI_FORMAT ToDxgiFormat(PixelFormat format); // DXGI_FORMAT_UNKNOWN for PixelFormat::None

	public:
		D3d12Texture();
		~D3d12Texture() override;

		bool BuildColor(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
		bool BuildDepth(uint32_t width, uint32_t height, const CreateInfo& createInfo) override;
		bool BuildBuffer(uint32_t width, uint32_t height, const uint8_t* buffer, const CreateInfo& createInfo) override;

		Color ReadPixel(const Vector2& position) const override;
		void  CaptureReadback(CommandBuffer* commandBuffer) override;

		ID3D12Resource* GetResource() const;

		// Only for a texture built with BuildColor
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

		// Only for a texture built with BuildDepth
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

		// The state of the texture is the one the last recorded command buffer leaves it in:
		// command buffers have to be submitted in the order they are recorded
		void Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);

	private:
		bool CreateResource(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state, uint32_t mipLevelCount = 1);
		bool CreateReadbackBuffer();
		void Release();

	private:
		ComPtr<ID3D12Resource> _resource;
		D3D12_RESOURCE_STATES  _state = D3D12_RESOURCE_STATE_COMMON;

		uint32_t _renderTargetViewIndex = D3d12DescriptorHeap::InvalidIndex;
		uint32_t _depthStencilViewIndex = D3d12DescriptorHeap::InvalidIndex;

		// CPU-visible copy of the texture content, kept up to date once per frame by CaptureReadback,
		// so ReadPixel() can be a plain memory read instead of an immediate, blocking GPU round-trip.
		ComPtr<ID3D12Resource>             _readbackBuffer;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT _readbackFootprint = {};
		void*                              _readbackMappedData = nullptr;
		bool                               _readbackReady = false;
	};
}
