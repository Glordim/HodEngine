#pragma once

#include "../Renderer.h"

#include <vector>

#include <D3d12.h>
#include <dxgi1_6.h>

#include <windows.h>

#include "D3d12GpuDevice.h"

#include <wrl.h>
using namespace Microsoft::WRL;

namespace HOD
{
    class RendererDirectX12 : public Renderer
    {
    public:
        RendererDirectX12();
        virtual ~RendererDirectX12();

        virtual bool Init(APPLICATION::Application* pApplication, bool enableValidationLayers) override;
        virtual bool SetupImGui() override;

        virtual bool GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;

        virtual bool BuildPipeline(GpuDevice* gpuDevice) override;

        virtual bool SubmitRenderQueue(RenderQueue& renderQueue) override;

        virtual bool ResizeSwapChain() override;

        virtual bool AcquireNextImageIndex() override;
        virtual bool SwapBuffer() override;

        virtual Mesh* CreateMesh(const std::string& path) override;
        virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) override;
        virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true) override;
        virtual MaterialInstance* CreateMaterialInstance(Material* material) override;
        virtual Texture* CreateTexture(const std::string& path) override;

    private:
        RECT g_WindowRect;

        HWND hWnd;

        D3d12GpuDevice* selectedGpu;
        std::vector<D3d12GpuDevice> availableGpu;

        // DirectX 12 Objects
        ComPtr<ID3D12Debug> debugInterface;
        ComPtr<IDXGIFactory7> dxgiFactory;
        ComPtr<ID3D12Device5> device;
        ComPtr<ID3D12CommandQueue> commandQueue;
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ComPtr<IDXGISwapChain4> swapChain;
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;
        std::vector<ComPtr<ID3D12Resource>> backBuffers;
    };
}
