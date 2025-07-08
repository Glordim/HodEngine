#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/D3d12/RendererDirectX12.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Shader.hpp"
#include "HodEngine/Renderer/RHI/D3d12/D3d12Material.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

namespace hod::renderer
{
	_SingletonOverrideConstructor(RendererDirectX12)
	: Renderer()
	{

	}

	RendererDirectX12::~RendererDirectX12()
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererDirectX12::Init(window::Window* mainWindow, uint32_t physicalDeviceIdentifier)
	{
		bool enableValidationLayers = true;
		if (enableValidationLayers == true)
		{
			HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&_debugInterface));
			if (FAILED(result) == true)
			{
				OUTPUT_ERROR("D3d12: Validation layers requested, but not available!");
				OUTPUT_ERROR("D3d12: Validation layers have been disabled");
				enableValidationLayers = false;
			}
			else
			{
				_debugInterface->EnableDebugLayer();
			}
		}
		UINT createFactoryFlags = 0;

		if (enableValidationLayers == true)
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

		HRESULT result = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&_dxgiFactory));
		if (FAILED(result) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to Create DXGI factory");
			return false;
		}

		Vector<GpuDevice*> physicalDevices;
		GetAvailableGpuDevices(&physicalDevices);
		_selectedGpu = &_availableGpu[0];

		if (FAILED(D3D12CreateDevice(_selectedGpu->adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device))))
		{
			OUTPUT_ERROR("D3d12: Unable to create Device!");
			return false;
		}

		if (SUCCEEDED(_device.As(&_infoQueue)) == true)
		{
			_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

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

			if (FAILED(_infoQueue->PushStorageFilter(&NewFilter)) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to setup debug InfoQueue!");
				return false;
			}
		}

		return true;
	}

	void RendererDirectX12::OutputErrors()
	{
		if (!_infoQueue) return;

		UINT64 numMessages = _infoQueue->GetNumStoredMessages();

		for (UINT64 i = 0; i < numMessages; ++i)
		{
			SIZE_T messageLength = 0;
			HRESULT hr = _infoQueue->GetMessage(i, nullptr, &messageLength);
			if (FAILED(hr)) continue;

			D3D12_MESSAGE* pMessage = (D3D12_MESSAGE*)malloc(messageLength);
			if (!pMessage) continue;

			hr = _infoQueue->GetMessage(i, pMessage, &messageLength);
			if (FAILED(hr))
			{
				free(pMessage);
				continue;
			}

			// Envoie vers ton système d'output perso
			OUTPUT_ERROR("D3D12: {}", pMessage->pDescription);

			free(pMessage);
		}

		// Une fois lu, on vide la queue pour éviter doublons
		_infoQueue->ClearStoredMessages();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererDirectX12::GetAvailableGpuDevices(Vector<GpuDevice*>* availableDevices)
	{
		if (availableDevices == nullptr)
			return false;

		if (_availableGpu.empty() == true)
		{
			UINT adaptaterIndex = 0;

			HRESULT result = S_OK;

			while (true)
			{
				ComPtr<IDXGIAdapter1> adapter = nullptr;

				result = _dxgiFactory->EnumAdapterByGpuPreference(adaptaterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
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

				_availableGpu.push_back(device);
			}
		}

		size_t avalaibleDeviceCount = _availableGpu.size();
		availableDevices->resize(avalaibleDeviceCount);

		for (size_t i = 0; i < avalaibleDeviceCount; ++i)
		{
			availableDevices->at(i) = (GpuDevice*)(&_availableGpu[i]);
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	bool RendererDirectX12::BuildPipeline(Context* context, uint32_t physicalDeviceIdentifier)
	{
		/*
		_selectedGpu = (D3d12GpuDevice*)gpuDevice;

		HRESULT result = D3D12CreateDevice(_selectedGpu->adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device));
		if (FAILED(result) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to create Device!");
			return false;
		}

		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(_device.As(&pInfoQueue)) == true)
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
				OUTPUT_ERROR("D3d12: Unable to setup debug InfoQueue!");
				return false;
			}
		}

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		if (FAILED(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue))) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to create Command queue!");
			return false;
		}

		int width = 0;
		int height = 0;

		HWND hwnd = NULL; // TODO Get from Application

		RECT rect;
		if (GetWindowRect(hwnd, &rect) == TRUE)
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		else
		{
			width = 800;
			height = 600;
		}

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		// It is recommended to always allow tearing if tearing support is available.
		swapChainDesc.Flags = 0; // CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> swapChain1 = nullptr;

		if (FAILED(_dxgiFactory->CreateSwapChainForHwnd(_commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1)) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to create Swap Chain!");
			return false;
		}

		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
		// will be handled manually.
		if (FAILED(_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to disable Alt+Enter for this Window!");
		}

		swapChain1.As(&_swapChain);

		D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
		descHeap.NumDescriptors = 15; // TODO
		descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		if (FAILED(_device->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&_descriptorHeap))) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to create Descriptor Heap!");
			return false;
		}

		UINT rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

		_backBuffers.resize(2);

		for (size_t i = 0; i < 2; ++i)
		{
			ComPtr<ID3D12Resource> backBuffer = nullptr;
			if (FAILED(_swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffer))) == true)
			{
				OUTPUT_ERROR("D3d12: Unable to retreive BackBuffer from SwapChain!");
				return false;
			}

			_device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

			_backBuffers[i] = backBuffer;

			rtvHandle.ptr += rtvDescriptorSize;
		}

		if (FAILED(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator))) == true)
		{
			OUTPUT_ERROR("D3d12: Unable to creare Command Allocator!");
			return false;
		}
		*/
		return true;
	}

	/*
				ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

				if (FAILED(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))) == true)
				{
					OUTPUT_ERROR("D3d12: Unable to creare Command List!");
					return false;
				}

				//commandList->SetGraphicsRootSignature(m_rootSignature.Get());
				//commandList->RSSetViewports(1, &viewport);
				//commandList->RSSetScissorRects(1, &scissor);

				D3D12_RESOURCE_TRANSITION_BARRIER transition;
				transition.pResource = _backBuffers[0].Get();
				transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

				D3D12_RESOURCE_BARRIER barrier;
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Transition = transition;
				commandList->ResourceBarrier(1, &barrier);

				D3D12_CPU_DESCRIPTOR_HANDLE rtv(_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

				commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

				FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

				commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

				transition.pResource = _backBuffers[0].Get();
				transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Transition = transition;
				commandList->ResourceBarrier(1, &barrier);

				if (FAILED(commandList->Close()) == true)
				{
					OUTPUT_ERROR("D3d12: Unable to close Command List!");
					return false;
				}

				ID3D12CommandList* const commandLists[] = {
					commandList.Get()
				};
				_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

				_swapChain->Present(0, 0);

				ComPtr<ID3D12Fence> fence = nullptr;
				if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))) == true)
				{
					OUTPUT_ERROR("D3d12: Unable to create Fence!");
					return false;
				}

				HANDLE fenceEvent = nullptr;
				fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (fenceEvent == nullptr)
				{
					OUTPUT_ERROR("D3d12: Unable to create Fence Event!");
					return false;
				}

				_commandQueue->Signal(fence.Get(), 1);
				fence->SetEventOnCompletion(1, fenceEvent);

				//WaitForSingleObject(fenceEvent, static_cast<DWORD>(std::numeric_limits<Uint32>().max()));

				_swapChain->Present1(0, 0, nullptr);
				*/

	ComPtr<ID3D12Device5> RendererDirectX12::GetDevice()
	{
		return _device;
	}

	bool RendererDirectX12::SubmitCommandBuffers(CommandBuffer** commandBuffers, uint32_t commandBufferCount, const Semaphore* signalSemaphore, const Semaphore* waitSemaphore, const Fence* fence)
	{
		return false;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Shader* RendererDirectX12::CreateShader(Shader::ShaderType type)
	{
		return DefaultAllocator::GetInstance().New<D3d12Shader>(type);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Material* RendererDirectX12::CreateMaterial(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, Material::PolygonMode polygonMode, Material::Topololy topololy, bool useDepth)
	{
		D3d12Material* mat = DefaultAllocator::GetInstance().New<D3d12Material>();

		if (mat->Build(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
		{
			DefaultAllocator::GetInstance().Delete(mat);
			return nullptr;
		}

		return mat;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	MaterialInstance* RendererDirectX12::CreateMaterialInstance(const Material* material)
	{
		return nullptr;
		/*
		if (material == nullptr)
		{
			// todo message, why not use ref ?
			return nullptr;
		}

		return DefaultAllocator::GetInstance().New<VkMaterialInstance>(*material);
		*/
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	CommandBuffer* RendererDirectX12::CreateCommandBuffer()
	{
		return nullptr;//return DefaultAllocator::GetInstance().New<CommandBufferVk>();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Buffer* RendererDirectX12::CreateBuffer(Buffer::Usage usage, uint32_t size)
	{
		return nullptr;//return DefaultAllocator::GetInstance().New<BufferVk>(usage, size);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	Texture* RendererDirectX12::CreateTexture()
	{
		return nullptr;//return DefaultAllocator::GetInstance().New<VkTexture>();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	RenderTarget* RendererDirectX12::CreateRenderTarget()
	{
		return nullptr;//return DefaultAllocator::GetInstance().New<VkRenderTarget>();
	}

	/// @brief 
	/// @return 
	Semaphore* RendererDirectX12::CreateSemaphore()
	{
		return nullptr;//return DefaultAllocator::GetInstance().New<SemaphoreVk>();
	}

	/// @brief 
	/// @return 
	Fence* RendererDirectX12::CreateFence()
	{
		return nullptr;//return DefaultAllocator::GetInstance().New<FenceVk>();
	}
}
