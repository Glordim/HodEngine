#pragma once
#include <HodEngine/HodEngine.hpp>

#if defined(_WIN32)

#include "HodEngine/Renderer/Renderer.hpp"

#include <vector>

#include <D3d12.h>
#include <dxgi1_6.h>

#include <Windows.h>

#include "D3d12GpuDevice.hpp"

#include <wrl.h>
using namespace Microsoft::WRL;

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API RendererDirectX12 : public Renderer
		{
		public:

												RendererDirectX12();
			virtual								~RendererDirectX12();

			bool								Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) override;

			bool								GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;

			bool								BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier = 0) override;

			bool								ResizeSwapChain() override;

			bool								AcquireNextImageIndex() override;
			bool								SwapBuffer() override;

			Shader*								CreateShader(Shader::ShaderType type) override;
			Material*							CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
			MaterialInstance*					CreateMaterialInstance(const Material* material) override;
			Texture*							CreateTexture() override;

		private:

			std::vector<D3d12GpuDevice>			_availableGpu;

			// DirectX 12 Objects
			ComPtr<ID3D12Debug>					_debugInterface;
			ComPtr<IDXGIFactory7>				_dxgiFactory;
			ComPtr<ID3D12Device5>				_device;
			ComPtr<ID3D12CommandQueue>			_commandQueue;
			ComPtr<ID3D12CommandAllocator>		_commandAllocator;
			ComPtr<IDXGISwapChain4>				_swapChain;
			ComPtr<ID3D12DescriptorHeap>		_descriptorHeap;
			std::vector<ComPtr<ID3D12Resource>>	_backBuffers;
		};
	}
}

#endif
