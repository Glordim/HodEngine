#include "RendererDirectX12.h"

RendererDirectX12::RendererDirectX12()
{

}

RendererDirectX12::~RendererDirectX12()
{

}

bool RendererDirectX12::Init(SDL_Window* window, bool enableValidationLayers)
{
    /*
    SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);
        HWND hWnd = wmInfo.info.win.window;

        ID3D12Debug* debugInterface = nullptr;
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));

        if (FAILED(hr))
        {
            SDL_Log("DirectX 12: Validation layers requested, but not available!");
            enableValidationLayers = false;
        }
        else
        {
            debugInterface->EnableDebugLayer();
        }

        ID3D12Device* device = nullptr;

        hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

        if (FAILED(hr))
        {
            SDL_Log("Unable to Create DirectX 12 device");
            return 1;
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS options;

        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, reinterpret_cast<void*>(&options), sizeof(options));

        if (FAILED(hr))
        {
            SDL_Log("DirectX 12: Unable to get FeatureSupport");
            return 1;
        }

        ID3D12CommandAllocator* commandListAllocator; //d3d12 command list allocator
        ID3D12CommandQueue* commandQueue; //d3d12 command queue
        IDXGIDevice2* DXGIDevice; //DXGI device
        IDXGISwapChain3* swapChain;   // the pointer to the swap chain interface
        ID3D12GraphicsCommandList* commandList;  //d3d12 command list
        ID3D12Fence* fence; //fence used by GPU to signal when command queue execution has finished
        //ID3D12Resource* mRenderTarget[g_bbCount];

        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&commandListAllocator);

        if (FAILED(hr))
        {
            SDL_Log("DirectX 12: Unable to Create Command allocator");
            return 1;
        }

        D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        hr = device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue);

        if (FAILED(hr))
        {
            SDL_Log("DirectX 12: Unable to Create Command queue");
            return 1;
        }

        const UINT backBufferCount = 4;

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.BufferCount = backBufferCount;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hWnd;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        //swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        IDXGIFactory2* dxgiFactory = nullptr;
        hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void**)&dxgiFactory);

        if (FAILED(hr))
        {
            SDL_Log("DirectX 12: Unable to Create DXGI factory");
            return 1;
        }

        hr = dxgiFactory->CreateSwapChain(commandQueue, &swapChainDesc, (IDXGISwapChain**)&swapChain);

        if (FAILED(hr))
        {
            SDL_Log("DirectX 12: Unable to Create SwapChain");
            return 1;
        }

        //increase max frame latency when required
        if (swapChainDesc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)
        {
            swapChain->SetMaximumFrameLatency(backBufferCount);
        }

        dxgiFactory->Release();
    */
    return false;
}

bool RendererDirectX12::CreateDevice()
{
    return false;
}
