#include "HodEngine/Renderer/Pch.hpp"
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

#include <HodEngine/Window/Event.hpp>
#include <HodEngine/Window/Window.hpp>

namespace hod
{
	namespace renderer
	{
		/// @brief
		_SingletonConstructor(Renderer)
		{
			MaterialManager::CreateInstance();
			PickingManager::CreateInstance();
			FontManager::CreateInstance();
			FontManager::GetInstance()->Init(); // todo catch error ?

			_frameResources.Resize(_frameInFlight);
			_texturesToDestroy.Resize(_frameInFlight);
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

			DefaultAllocator::GetInstance().Delete(_defaultWhiteTexture);
			_defaultWhiteTexture = nullptr;
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

					renderer::VertexInput vertexInput[1] = {
						{0, 0, renderer::VertexInput::Format::R32G32_SFloat},
						//{ 8, renderer::VertexInput::Format::R32G32_SFloat },
					    //{ 16, renderer::VertexInput::Format::A8B8G8R8_UNorm_Pack32 },
					};

					Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
					if (vertexShader->LoadFromIR(P2f_Unlit_Vertex, P2f_Unlit_Vertex_size, P2f_Unlit_Vertex_reflection, P2f_Unlit_Vertex_reflection_size) == false)
					{
						DefaultAllocator::GetInstance().Delete(vertexShader);
						return nullptr;
					}

					Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
					if (fragmentShader->LoadFromIR(P2f_Unlit_Fragment, P2f_Unlit_Fragment_size, P2f_Unlit_Fragment_reflection, P2f_Unlit_Fragment_reflection_size) == false)
					{
						DefaultAllocator::GetInstance().Delete(vertexShader);
						DefaultAllocator::GetInstance().Delete(fragmentShader);
						return nullptr;
					}

					_defaultMaterial = renderer->CreateMaterial(vertexInput, 1, vertexShader, fragmentShader);
					if (_defaultMaterial == nullptr)
					{
						DefaultAllocator::GetInstance().Delete(vertexShader);
						DefaultAllocator::GetInstance().Delete(fragmentShader);
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
					_overdrawnMaterial = MaterialManager::GetInstance()->GetData(MaterialManager::GetInstance()->CreateMaterial("SpriteOverdraw"));
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
					_wireframeMaterial = MaterialManager::GetInstance()->GetData(
						MaterialManager::GetInstance()->CreateMaterial("SpriteWireframe", Material::PolygonMode::Line, Material::Topololy::TRIANGLE));
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
			Vector<PresentationSurface*> presentationSurfaceToResize;
			for (PresentationSurface* presentationSurface : _presentationSurfaces)
			{
				window::Event event;
				uint32_t      eventIndex = 0;

				window::Window* window = presentationSurface->GetWindow();
				if (window->PollEvent(eventIndex, event, 1 << static_cast<uint8_t>(window::EventType::Resized)))
				{
					presentationSurfaceToResize.PushBack(presentationSurface);
				}
			}
			if (presentationSurfaceToResize.Empty() == false)
			{
				for (uint32_t frameSkipped = 0; frameSkipped < _frameInFlight; ++frameSkipped)
				{
					FrameResources& frameResources = GetCurrentFrameResources();
					frameResources.Wait();
					frameResources.DestroyAll();

					for (Texture* textureToDestroy : _texturesToDestroy[_frameIndex])
					{
						DefaultAllocator::GetInstance().Delete(textureToDestroy);
					}
					_texturesToDestroy[_frameIndex].Clear();

					++_frameCount;
					_frameIndex = _frameCount % _frameInFlight;
				}

				for (PresentationSurface* presentationSurface : presentationSurfaceToResize)
				{
					window::Window* window = presentationSurface->GetWindow();
					presentationSurface->Resize(window->GetWidth(), window->GetHeight());
				}
			}
			else
			{
				FrameResources& frameResources = GetCurrentFrameResources();
				frameResources.Wait();
				frameResources.DestroyAll();

				for (Texture* textureToDestroy : _texturesToDestroy[_frameIndex])
				{
					DefaultAllocator::GetInstance().Delete(textureToDestroy);
				}
				_texturesToDestroy[_frameIndex].Clear();
			}

			for (PresentationSurface* presentationSurface : _presentationSurfaces)
			{
				if (presentationSurface->AcquireNextImageIndex(GetCurrentFrameResources().GetImageAvalaibleSemaphore()) == false)
				{
					return false;
				}
			}
			return true;
		}

		void Renderer::DestroyTexture(Texture* texture)
		{
			_texturesToDestroy[_frameIndex].PushBack(texture);
		}

		PresentationSurface* Renderer::FindPresentationSurface(window::Window* window) const
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
}
