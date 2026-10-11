#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Buffer.hpp"

#include "HodEngine/RHI/D3d12/D3d12Api.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RHI_API D3d12Buffer : public Buffer
	{
	public:
		D3d12Buffer(Usage usage, uint32_t size);
		D3d12Buffer(const D3d12Buffer&) = delete;
		D3d12Buffer(D3d12Buffer&&) = delete;
		~D3d12Buffer() override;

		D3d12Buffer& operator=(const D3d12Buffer&) = delete;
		D3d12Buffer& operator=(D3d12Buffer&&) = delete;

	public:
		ID3D12Resource*           GetResource() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const;
		uint32_t                  GetSize() const;

		bool  Resize(uint32_t size) override;
		void* Lock() override;
		void  Unlock() override;

	private:
		void Release();

	private:
		ComPtr<ID3D12Resource> _resource;
	};
}
