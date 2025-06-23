#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include <HodEngine/Core/Rect.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::game
{
	CameraComponent* CameraComponent::_main = nullptr;

	DESCRIBE_REFLECTED_CLASS(CameraComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CameraComponent::_projection, "Projection");
		AddPropertyT(reflectionDescriptor, &CameraComponent::_near, "Near");
		AddPropertyT(reflectionDescriptor, &CameraComponent::_far, "Far");
		AddPropertyT(reflectionDescriptor, &CameraComponent::_fov, "Fov");
		AddPropertyT(reflectionDescriptor, &CameraComponent::_size, "Size", &CameraComponent::SetSize);
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
	/// @param renderView 
	void CameraComponent::SetupRenderView(renderer::RenderView& renderView)
	{
		Vector2 resolution = renderView.GetRenderResolution();

		Rect viewport;
		viewport._position.SetX(0);
		viewport._position.SetY(0);
		viewport._size.SetX(resolution.GetX());
		viewport._size.SetY(resolution.GetY());

		SetAspect(resolution.GetX() / resolution.GetY());

		Matrix4 projection = GetProjectionMatrix();
		Matrix4 view = Matrix4::Identity;

		Entity* entity = GetOwner();
		if (entity != nullptr)
		{
			NodeComponent* nodeComponent = entity->GetComponent<NodeComponent>();
			if (nodeComponent != nullptr)
			{
				view = nodeComponent->GetWorldMatrix();
			}
		}

		renderView.SetupCamera(projection, view, viewport);
	}

	/// @brief 
	/// @param size 
	void CameraComponent::SetSize(float size)
	{
		if (size != _size)
		{
			_size = size;
			_dirtyFlag = true;
		}
	}

	/// @brief 
	/// @return 
	float CameraComponent::GetSize() const
	{
		return _size;
	}
}
