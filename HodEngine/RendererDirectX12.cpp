#include "RendererDirectX12.h"

#include <SDL.h>
#include <SDL_syswm.h>

RendererDirectX12::RendererDirectX12()
{
    this->debugInterface = nullptr;
    this->dxgiFactory = nullptr;
}

RendererDirectX12::~RendererDirectX12()
{

}

bool RendererDirectX12::Init(SDL_Window* window, bool enableValidationLayers)
{
    if (enableValidationLayers == true)
    {
        HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&this->debugInterface));
        if (FAILED(result) == true)
        {
            fprintf(stderr, "D3d12: Validation layers requested, but not available!\n");
            fprintf(stderr, "D3d12: Validation layers have been disabled\n");
            enableValidationLayers = false;
        }
        else
        {
            this->debugInterface->EnableDebugLayer();
        }
    }

    UINT createFactoryFlags = 0;

    if (enableValidationLayers == true)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

    HRESULT result = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&this->dxgiFactory));
    if (FAILED(result) == true)
    {
        fprintf(stderr, "D3d12: Unable to Create DXGI factory\n");
        return false;
    }

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(window, &wmInfo) == SDL_FALSE)
    {
        fprintf(stderr, "SDL: Unable to get native Window instance!\n");
        return false;
    }

    this->hWnd = wmInfo.info.win.window;

    return true;
}

    /*
    HRESULT result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&this->device));
    if (FAILED(result) == true)
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

bool RendererDirectX12::GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices)
{
    if (availableDevices == nullptr)
        return false;

    if (this->availableGpu.empty() == true)
    {
        UINT adaptaterIndex = 0;

        HRESULT result = S_OK;
        
        while (true)
        {
            ComPtr<IDXGIAdapter1> adapter = nullptr;

            result = this->dxgiFactory->EnumAdapterByGpuPreference(adaptaterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
            ++adaptaterIndex;

            if (FAILED(result) == true)
                break;

            DXGI_ADAPTER_DESC1 desc;

            result = adapter->GetDesc1(&desc);
            if (FAILED(result) == true)
                continue;

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                continue;
            }

            D3d12GpuDevice device;
            device.adapter = adapter;
            device.name = desc.Description;
            device.vram = desc.DedicatedVideoMemory;
            device.score = desc.DedicatedVideoMemory;

            result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
            if (FAILED(result) == true)
                device.compatible = false;
            else
                device.compatible = true;

            this->availableGpu.push_back(device);            
        }
    }

    size_t avalaibleDeviceCount = this->availableGpu.size();
    availableDevices->resize(avalaibleDeviceCount);

    for (size_t i = 0; i < avalaibleDeviceCount; ++i)
    {
        availableDevices->at(i) = (GpuDevice*)(&this->availableGpu[i]);
    }

    return true;
}

bool RendererDirectX12::BuildPipeline(GpuDevice* gpuDevice)
{
    this->selectedGpu = (D3d12GpuDevice*)gpuDevice;

    HRESULT result = D3D12CreateDevice(this->selectedGpu->adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&this->device));
    if (FAILED(result) == true)
    {
        fprintf(stderr, "D3d12: Unable to create Device!\n");
        return false;
    }

    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(this->device.As(&pInfoQueue)) == true)
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // Suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        if (FAILED(pInfoQueue->PushStorageFilter(&NewFilter)) == true)
        {
            fprintf(stderr, "D3d12: Unable to setup debug InfoQueue!\n");
            return false;
        }
    }

    /*
    ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));
    */

    return true;
}

bool RendererDirectX12::SubmitRenderQueue(RenderQueue& renderQueue)
{
    return false;
}

bool RendererDirectX12::ResizeSwapChain()
{
    return false;
}

bool RendererDirectX12::AcquireNextImageIndex()
{
    return false;
}

bool RendererDirectX12::SwapBuffer()
{
    return false;
}

Shader* RendererDirectX12::CreateShader(const std::string& path, Shader::ShaderType type)
{
    return nullptr;
}

Material* RendererDirectX12::CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy)
{
    return nullptr;
}

MaterialInstance* RendererDirectX12::CreateMaterialInstance(Material* material)
{
    return nullptr;
}

Mesh* RendererDirectX12::CreateMesh(const std::string& path)
{
    return nullptr;
}
