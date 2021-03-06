#include "CameraComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/gtc/matrix_transform.hpp"

#include "SceneComponent.h"

//#include "AntTweakBar.h"

#include <Renderer/src/Renderer.h>
#include <Renderer/src/RenderQueue.h>
#include "../RenderQueueHelper.h"

#include <Application/src/Application.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		CameraComponent::CameraComponent(Actor* actor)
			: Component(actor)
			, _fov(60.0f)
			, _aspect(1920.0f / 1080.0f)
			, _near(0.01f)
			, _far(1000.0f)
			, _perspective(true)
			, _dirtyFlag(true)
			, _hdriMat(nullptr)
			, _hdriTexture(nullptr)
		{
			APPLICATION::Application* app = APPLICATION::Application::GetInstance();

			int width = app->GetWidth();
			int height = app->GetHeight();

			_aspect = (float)width / (float)height;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CameraComponent::DrawImGui()
		{
			//TwAddSeparator(tweakBar, "Camera", "");
			//TwAddVarRW(tweakBar, "fov", TW_TYPE_FLOAT, &_fov, "");
			//TwAddVarRW(tweakBar, "near", TW_TYPE_FLOAT, &_near, "");
			//TwAddVarRW(tweakBar, "far", TW_TYPE_FLOAT, &_far, "");
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
		void CameraComponent::SetHdriMaterial(RENDERER::MaterialInstance* hdriMat, RENDERER::Texture* hdriTexture)
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
					_projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, _near, _far);
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
			RENDERER::RenderQueue renderQueue;

			renderQueue.SetCameraPos(GetActor()->GetComponent<SceneComponent>()->GetPosition());
			renderQueue.SetViewMatrix(glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix()));
			renderQueue.SetProjMatrix(GetProjectionMatrix());

			renderQueue.SetClearFlag(RENDERER::RenderQueue::ClearFlag::COLOR | RENDERER::RenderQueue::ClearFlag::DEPTH);

			renderQueue.SetHdriMaterial(_hdriMat);
			renderQueue.SetHdriTexture(_hdriTexture);

			RenderQueueHelper::AddSceneComponent(renderQueue, scene.GetRoot(), true);
			RenderQueueHelper::AddScenePhysicsDebug(renderQueue, &scene);
			RenderQueueHelper::AddDebugLines(renderQueue, &scene);

			RENDERER::Renderer* renderer = RENDERER::Renderer::GetInstance();
			renderer->SubmitRenderQueue(renderQueue);
		}
	}
}
