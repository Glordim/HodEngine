#include "CameraComponent.h"

#include <HodEngine/Core/Rect.h>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/gtc/matrix_transform.hpp"

#include "SceneComponent.h"

#include <HodEngine/Renderer/Renderer.h>
#include <HodEngine/Renderer/RenderQueue.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.h>
#include "../RenderQueueHelper.h"


//#include <HodEngine/Window/src/Applications/GraphicApplications/GraphicApplication.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		CameraComponent::CameraComponent(Actor* actor)
			: Component(actor)
			, _fov(60.0f)
			, _aspect(1920.0f / 1080.0f)
			, _near(-100.0f)
			, _far(100.0f)
			, _perspective(false)
			, _dirtyFlag(true)
			, _hdriMat(nullptr)
			, _hdriTexture(nullptr)
		{
			/*
			GraphicApplication* graphicApplication = GraphicApplication::GetInstance();

			int width = graphicApplication->GetWidth();
			int height = graphicApplication->GetHeight();

			_aspect = (float)width / (float)height;
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CameraComponent::DrawImGui()
		{

		}

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
		const glm::mat4& CameraComponent::GetProjectionMatrix()
		{
			if (_dirtyFlag == true)
			{
				if (_perspective == true)
				{
					_projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
				}
				else
				{
					_projectionMatrix = glm::ortho(-_aspect * 0.5f, _aspect * 0.5f, -0.5f, 0.5f, _near, _far);
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
		void CameraComponent::PushToRenderQueue(renderer::RenderQueue* renderQueue)
		{
			if (renderQueue == nullptr)
			{
				renderQueue = renderer::Renderer::GetInstance()->GetRenderQueue();
			}

			CORE::Rect viewport;
			viewport._position.x = 0;
			viewport._position.y = 0;
			viewport._size.x = 1.0f;
			viewport._size.y = 1.0f;
			renderQueue->PushRenderCommand(new renderer::RenderCommandSetCameraSettings(GetProjectionMatrix(), glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix()), viewport));
		}
	}
}
