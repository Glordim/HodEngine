#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Core/Vector.hpp"

#include <D3d12.h>
#include <dxgi1_6.h>

#include <Windows.h>

#include "D3d12GpuDevice.hpp"

#include <wrl.h>
using namespace Microsoft::WRL;

#undef CreateSemaphore

namespace hod::renderer
{
	class HOD_RENDERER_API RendererDirectX12 : public Renderer
	{
		_SingletonOverride(RendererDirectX12)

	protected:

		~RendererDirectX12() override;

	public:

		bool								Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier = 0) override;

		bool								GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices) override;

		bool								BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier = 0) override;
		
		bool SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore = nullptr, const Semaphore* waitSemaphore = nullptr, const Fence* fence = nullptr) override;

		CommandBuffer* CreateCommandBuffer() override;
		Buffer* CreateBuffer(Buffer::Usage usage, uint32_t size) override;
		Shader* CreateShader(Shader::ShaderType type) override;
		Material* CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
		MaterialInstance* CreateMaterialInstance(const Material* material) override;
		Texture* CreateTexture() override;
		RenderTarget* CreateRenderTarget() override;
		Semaphore* CreateSemaphore() override;
		Fence* CreateFence() override;

	private:

		Vector<D3d12GpuDevice>				_availableGpu;

		// DirectX 12 Objects
		ComPtr<ID3D12Debug>					_debugInterface = nullptr;
		ComPtr<IDXGIFactory7>				_dxgiFactory = nullptr;
		ComPtr<ID3D12Device5>				_device = nullptr;
		ComPtr<ID3D12CommandQueue>			_commandQueue = nullptr;
		ComPtr<ID3D12CommandAllocator>		_commandAllocator = nullptr;
		ComPtr<IDXGISwapChain4>				_swapChain = nullptr;
		ComPtr<ID3D12DescriptorHeap>		_descriptorHeap = nullptr;
		Vector<ComPtr<ID3D12Resource>>		_backBuffers;
	};
}
