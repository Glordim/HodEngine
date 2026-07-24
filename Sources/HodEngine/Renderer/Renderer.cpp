#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

#include "HodEngine/Renderer/Font/FontManager.hpp"
#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/PickingManager.hpp"
#include "HodEngine/Renderer/RenderView.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"

#include "Shader/P2f_Unlit_Fragment.hpp"
#include "Shader/P2f_Unlit_Vertex.hpp"

#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include "HodEngine/Renderer/FrameResources.hpp"

#include <HodEngine/Window/Window.hpp>
#include <algorithm>

namespace hod::inline renderer
{
	/// @brief
	_SingletonConstructor(Renderer)
	{
		MaterialManager::CreateInstance();
		PickingManager::CreateInstance();
		FontManager::CreateInstance();
		FontManager::GetInstance()->Init(); // todo catch error ?

		_frameResources.Resize(_frameInFlight);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Renderer::~Renderer()
	{
		FontManager::DestroyInstance();
		PickingManager::DestroyInstance();
		MaterialManager::DestroyInstance();
	}

	/// @brief
	void Renderer::Clear()
	{
		MaterialManager::GetInstance()->Clear();

		DefaultAllocator::GetInstance().Delete(_overdrawnMaterialInstance);
		_overdrawnMaterialInstance = nullptr;
		DefaultAllocator::GetInstance().Delete(_overdrawnMaterial);
		_overdrawnMaterial = nullptr;

		DefaultAllocator::GetInstance().Delete(_wireframeMaterialInstance);
		_wireframeMaterialInstance = nullptr;
		DefaultAllocator::GetInstance().Delete(_wireframeMaterial);
		_wireframeMaterial = nullptr;

		DefaultAllocator::GetInstance().Delete(_defaultMaterialInstance);
		_defaultMaterialInstance = nullptr;
		DefaultAllocator::GetInstance().Delete(_defaultMaterial);
		_defaultMaterial = nullptr;
		DefaultAllocator::GetInstance().Delete(_defaultVertexShader);
		_defaultVertexShader = nullptr;
		DefaultAllocator::GetInstance().Delete(_defaultFragmentShader);
		_defaultFragmentShader = nullptr;

		DefaultAllocator::GetInstance().Delete(_defaultWhiteTexture);
		_defaultWhiteTexture = nullptr;

		for (uint32_t i = 0; i < _frameInFlight; ++i)
		{
			_frameResources[i].Wait();
			_frameResources[i].DestroyAll();
		}

		for (PresentationSurface* presentationSurface : _presentationSurfaces)
		{
			DefaultAllocator::GetInstance().Delete(presentationSurface);
		}
		_presentationSurfaces.Clear();
	}

	/*
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool Renderer::Init()
	{
		DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

		pDebugLayer->RegisterDebugWindow(&_rendererDebugWindow);

		return true;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void Renderer::Clear()
	{
		DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::GetInstance();

		pDebugLayer->UnregisterDebugWindow(&_rendererDebugWindow);
	}
	*/
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	Renderer::VisualizationMode Renderer::GetVisualizationMode() const
	{
		return _visualizationMode;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void Renderer::SetVisualizationMode(VisualizationMode visualizationMode)
	{
		_visualizationMode = visualizationMode;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MaterialInstance* Renderer::GetDefaultMaterialInstance()
	{
		if (_defaultMaterialInstance == nullptr)
		{
			if (_defaultMaterial == nullptr)
			{
				Renderer* renderer = Renderer::GetInstance();

				VertexInput vertexInput[1] = {
					{0, 0, VertexInput::Format::R32G32_SFloat},
					//{ 8, VertexInput::Format::R32G32_SFloat },
					//{ 16, VertexInput::Format::A8B8G8R8_UNorm_Pack32 },
				};

				_defaultVertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
				if (_defaultVertexShader->LoadFromIR(P2f_Unlit_Vertex, P2f_Unlit_Vertex_size, P2f_Unlit_Vertex_reflection, P2f_Unlit_Vertex_reflection_size) == false)
				{
					DefaultAllocator::GetInstance().Delete(_defaultVertexShader);
					_defaultVertexShader = nullptr;
					return nullptr;
				}

				_defaultFragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
				if (_defaultFragmentShader->LoadFromIR(P2f_Unlit_Fragment, P2f_Unlit_Fragment_size, P2f_Unlit_Fragment_reflection, P2f_Unlit_Fragment_reflection_size) == false)
				{
					DefaultAllocator::GetInstance().Delete(_defaultVertexShader);
					DefaultAllocator::GetInstance().Delete(_defaultFragmentShader);
					_defaultVertexShader = nullptr;
					_defaultFragmentShader = nullptr;
					return nullptr;
				}

				_defaultMaterial = renderer->CreateMaterial(vertexInput, 1, _defaultVertexShader, _defaultFragmentShader);
				if (_defaultMaterial == nullptr)
				{
					DefaultAllocator::GetInstance().Delete(_defaultVertexShader);
					DefaultAllocator::GetInstance().Delete(_defaultFragmentShader);
					_defaultVertexShader = nullptr;
					_defaultFragmentShader = nullptr;
					return nullptr;
				}
			}

			_defaultMaterialInstance = CreateMaterialInstance(_defaultMaterial);
		}

		return _defaultMaterialInstance;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MaterialInstance* Renderer::GetOverdrawMaterialInstance()
	{
		if (_overdrawnMaterialInstance == nullptr)
		{
			if (_overdrawnMaterial == nullptr)
			{
				//_overdrawnMaterial = MaterialManager::GetInstance()->CreateMaterial("SpriteOverdraw"); // TODO
			}

			_overdrawnMaterialInstance = CreateMaterialInstance(_overdrawnMaterial);
		}

		return _overdrawnMaterialInstance;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	MaterialInstance* Renderer::GetWireframeMaterialInstance()
	{
		if (_wireframeMaterialInstance == nullptr)
		{
			if (_wireframeMaterial == nullptr)
			{
				// TODO
				//_wireframeMaterial = MaterialManager::GetInstance()->GetData(
				//	MaterialManager::GetInstance()->CreateMaterial("SpriteWireframe", Material::PolygonMode::Line, Material::Topololy::TRIANGLE));
			}

			_wireframeMaterialInstance = CreateMaterialInstance(_wireframeMaterial);
		}

		return _wireframeMaterialInstance;
	}

	/// @brief
	/// @return
	Texture* Renderer::GetDefaultWhiteTexture()
	{
		if (_defaultWhiteTexture == nullptr)
		{
			uint8_t pixels[4 * 2 * 2] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

			_defaultWhiteTexture = CreateTexture();
			_defaultWhiteTexture->BuildBuffer(2, 2, pixels, Texture::CreateInfo());
		}
		return _defaultWhiteTexture;
	}

	/*
	void Renderer::PushRenderView(RenderView& renderView, bool autoDestroyAfterFrame)
	{
		renderView.SetAutoDestroy(autoDestroyAfterFrame);
		_renderViews.push_back(&renderView);
	}

	void Renderer::RenderViews()
	{
		// todo sort

		Semaphore* semaphore = nullptr;
		Context*   context = nullptr;
		for (RenderView* renderView : _renderViews)
		{
			if (renderView->GetContext())
			{
				context = renderView->GetContext();
				if (semaphore == nullptr)
				{
					semaphore = (Semaphore*)renderView->GetContext()->GetImageAvailableSempahore();
				}
				renderView->Execute(semaphore);
				semaphore = renderView->GetRenderFinishedSemaphore();
			}
			else
			{
				renderView->Execute();
			}
		}
		context->AddSemaphoreToSwapBuffer(semaphore);
	}

	void Renderer::WaitViews()
	{
		for (RenderView* renderView : _renderViews)
		{
			renderView->Wait();
		}

		for (RenderView* renderView : _renderViews)
		{
			if (renderView->IsAutoDestroy())
			{
				DefaultAllocator::GetInstance().Delete(renderView);
			}
		}
		_renderViews.Clear();
	}
		*/

	void Renderer::Render()
	{
		FrameResources& frameResources = GetCurrentFrameResources();
		frameResources.Submit();

		++_frameCount;
		_frameIndex = _frameCount % _frameInFlight;
	}

	FrameResources& Renderer::GetCurrentFrameResources()
	{
		return _frameResources[_frameIndex];
	}

	bool Renderer::AcquireNextFrame()
	{
		// Secondary presentation surfaces (e.g. ImGui viewports) are still acquired lazily, the
		// first time a RenderView targets them this frame (see FrameResources::AcquireSurface,
		// called from RenderView::Prepare). This avoids acquiring images for surfaces that end up
		// not being rendered this frame, and allows surfaces created mid-frame to be acquired
		// correctly.
		// The main surface is acquired eagerly here instead: vkAcquireNextImageKHR is the real
		// vsync/FIFO pacing point, and doing it before input is polled (see GraphicApplication::
		// EngineLoop) keeps input sampling as close as possible to that pacing point, instead of
		// having input go stale while queued behind it.
		FrameResources& frameResources = GetCurrentFrameResources();
		frameResources.Wait();
		frameResources.DestroyAll();

		FlushDeferredDeletions(_frameIndex);

		if (_mainPresentationSurface != nullptr)
		{
			frameResources.AcquireSurface(_mainPresentationSurface);
		}

		return true;
	}

	void Renderer::DestroyPresentationSurface(window::Window* window)
	{
		for (auto it = _presentationSurfaces.Begin(); it != _presentationSurfaces.End(); ++it)
		{
			if ((*it)->GetWindow() == window)
			{
				WaitIdle();
				PresentationSurface* presentationSurface = *it;
				_presentationSurfaces.Erase(it);
				DefaultAllocator::GetInstance().Delete(presentationSurface);
				return;
			}
		}
	}

	PresentationSurface* Renderer::FindPresentationSurface(Window* window) const
	{
		for (PresentationSurface* presentationSurface : _presentationSurfaces)
		{
			if (presentationSurface->GetWindow() == window)
			{
				return presentationSurface;
			}
		}
		return nullptr;
	}

	uint32_t Renderer::GetFrameIndex() const
	{
		return _frameIndex;
	}

	uint32_t Renderer::GetFrameInFlightCount() const
	{
		return _frameInFlight;
	}
}
