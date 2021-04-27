#pragma once

#include "../../Renderer.h"

#include <vector>

#include <D3d12.h>
#include <dxgi1_6.h>

#include <windows.h>

#include "D3d12GpuDevice.h"

#include <wrl.h>
using namespace Microsoft::WRL;

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RendererDirectX12 : public Renderer
		{
		public:

												RendererDirectX12();
			virtual								~RendererDirectX12();

			bool								Init(APPLICATION::Application* pApplication, bool enableValidationLayers) override;
			bool								SetupImGui() override;

			bool								GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;

			bool								BuildPipeline(GpuDevice* gpuDevice) override;

			bool								ResizeSwapChain() override;

			bool								AcquireNextImageIndex() override;
			bool								SwapBuffer() override;

			Shader*								CreateShader(const std::string& path, Shader::ShaderType type) override;
			Material*							CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
			MaterialInstance*					CreateMaterialInstance(const Material* material) override;
			Texture*							CreateTexture(const std::string& path) override;

		private:

			RECT								_g_WindowRect;

			HWND								_hWnd;

			D3d12GpuDevice*						_selectedGpu;
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
