#include "HodEngine/Game/Components/CameraComponent.hpp"

#include <HodEngine/Core/Rect.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp>
#include "../RenderQueueHelper.hpp"

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
			/*
		CameraComponent::CameraComponent(const std::weak_ptr<Entity>& entity)
			: Component(entity)
			, _fov(60.0f)
			, _aspect(1920.0f / 1080.0f)
			, _near(-100.0f)
			, _far(100.0f)
			, _perspective(false)
			, _dirtyFlag(true)
			, _hdriMat(nullptr)
			, _hdriTexture(nullptr)
		{
			GraphicApplication* graphicApplication = GraphicApplication::GetInstance();

			int width = graphicApplication->GetWidth();
			int height = graphicApplication->GetHeight();

			_aspect = (float)width / (float)height;
		}
			*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* CameraComponent::GetType() const
		{
			return "Camera";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CameraComponent::SetHdriMaterial(renderer::MaterialInstance* hdriMat, renderer::Texture* hdriTexture)
		{
			_hdriMat = hdriMat;
			_hdriTexture = hdriTexture;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Matrix4& CameraComponent::GetProjectionMatrix()
		{
			if (_dirtyFlag == true)
			{
				if (_perspective == true)
				{
					_projectionMatrix = Matrix4::Identity;//glm::perspective(glm::radians(_fov), _aspect, _near, _far);
				}
				else
				{
					_projectionMatrix = Matrix4::OrthogonalProjection(-_aspect * 0.5f, _aspect * 0.5f, -0.5f, 0.5f, _near, _far);
				}

				_dirtyFlag = false;
			}

			return _projectionMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CameraComponent::Render(Scene& scene)
		{
			/*
			renderer::RenderQueue renderQueue;

			renderQueue.SetCameraPos(GetActor()->GetComponent<SceneComponent>()->GetPosition());
			renderQueue.SetViewMatrix(glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix()));
			renderQueue.SetProjMatrix(GetProjectionMatrix());

			renderQueue.SetClearFlag(renderer::RenderQueue::ClearFlag::COLOR | renderer::RenderQueue::ClearFlag::DEPTH);

			renderQueue.SetHdriMaterial(_hdriMat);
			renderQueue.SetHdriTexture(_hdriTexture);

			RenderQueueHelper::AddSceneComponent(renderQueue, scene.GetRoot(), true);
			RenderQueueHelper::AddScenePhysicsDebug(renderQueue, &scene);
			RenderQueueHelper::AddDebugLines(renderQueue, &scene);

			renderer::Renderer* renderer = renderer::Renderer::GetInstance();
			renderer->SubmitRenderQueue(renderQueue);
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CameraComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			Rect viewport;
			viewport._position.SetX(0);
			viewport._position.SetY(0);
			viewport._size.SetX(1.0f);
			viewport._size.SetY(1.0f);
			//renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(GetProjectionMatrix(), glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix()), viewport));
		}
	}
}
