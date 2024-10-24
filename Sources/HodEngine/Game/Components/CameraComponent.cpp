#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include <HodEngine/Core/Rect.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandSetCameraSettings.hpp>
#include "../RenderQueueHelper.hpp"

namespace hod::game
{
	CameraComponent* CameraComponent::_main = nullptr;

	DESCRIBE_REFLECTED_CLASS(CameraComponent, Component)
	{
		AddPropertyT(this, &CameraComponent::_projection, "Projection");
		AddPropertyT(this, &CameraComponent::_near, "Near");
		AddPropertyT(this, &CameraComponent::_far, "Far");
		AddPropertyT(this, &CameraComponent::_fov, "Fov");
		AddPropertyT(this, &CameraComponent::_size, "Size");
	}

	/// @brief 
	void CameraComponent::OnAwake()
	{
		_main = this;
	}

	/// @brief 
	/// @return 
	const Matrix4& CameraComponent::GetProjectionMatrix()
	{
		if (_dirtyFlag == true)
		{
			if (_projection == Projection::Perpective)
			{
				_projectionMatrix = Matrix4::Identity; // todo
			}
			else
			{
				_projectionMatrix = Matrix4::OrthogonalProjection(-_size * _aspect, _size * _aspect, -_size, _size, _near, _far);
			}

			_dirtyFlag = false;
		}

		return _projectionMatrix;
	}

	/// @brief 
	/// @param aspect 
	void CameraComponent::SetAspect(float aspect)
	{
		if (aspect != _aspect)
		{
			_aspect = aspect;
			_dirtyFlag = true;
		}
	}

	/// @brief 
	/// @param scene 
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

	/// @brief 
	/// @param renderQueue 
	void CameraComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
	{
		float size = 5.0f;

		Rect viewport;
		viewport._position.SetX(0);
		viewport._position.SetY(0);
		viewport._size.SetX((float)renderQueue.GetRenderWidth());
		viewport._size.SetY((float)renderQueue.GetRenderHeight());

		SetAspect((float)renderQueue.GetRenderWidth() / (float)renderQueue.GetRenderHeight());

		Matrix4 projection = GetProjectionMatrix();
		Matrix4 view = Matrix4::Identity;

		std::shared_ptr<Entity> entity = GetEntity();
		if (entity != nullptr)
		{
			std::shared_ptr<NodeComponent> nodeComponent = entity->GetComponent<NodeComponent>();
			if (nodeComponent != nullptr)
			{
				view = nodeComponent->GetWorldMatrix();
			}
		}
		renderQueue.PushRenderCommand(new renderer::RenderCommandSetCameraSettings(projection, view, viewport));
	}
}
