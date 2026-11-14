#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/Components/CameraComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp"

#include <HodEngine/Math/Rect.hpp>

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCategory.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDescription.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline renderer
{
	CameraComponent* CameraComponent::_main = nullptr;

	DESCRIBE_REFLECTED_ENUM(CameraComponent::Projection)
	{
		HOD_ENUM_VALUE(Orthographic);
		HOD_ENUM_VALUE(Perpective);
	}

	DESCRIBE_REFLECTED_CLASS(CameraComponent)
	{
		HOD_TRAIT(ReflectionTraitIcon, ICON_MDI_CAMERA);
		HOD_TRAIT(ReflectionTraitCategory, "Rendering");
		HOD_TRAIT(ReflectionTraitDescription, "Point of view the world is rendered from");
		HOD_TRAIT(ReflectionTraitRequireComponent, &Node2dComponent::GetReflectionDescriptor);

		HOD_PROPERTY(_projection);
		HOD_PROPERTY(_near);
		HOD_PROPERTY(_far);
		HOD_PROPERTY(_fov);
		HOD_PROPERTY(_size, &CameraComponent::SetSize);
	}

	/// @brief
	void CameraComponent::OnAwake()
	{
		_main = this;
	}

	void CameraComponent::OnDestruct()
	{
		_main = nullptr;
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
		(void)scene; // TODO
					 /*
		             RenderQueue renderQueue;
		     
		             renderQueue.SetCameraPos(GetActor()->GetComponent<SceneComponent>()->GetPosition());
		             renderQueue.SetViewMatrix(glm::inverse(GetActor()->GetComponent<SceneComponent>()->GetModelMatrix()));
		             renderQueue.SetProjMatrix(GetProjectionMatrix());
		     
		             renderQueue.SetClearFlag(RenderQueue::ClearFlag::COLOR | RenderQueue::ClearFlag::DEPTH);
		     
		             renderQueue.SetHdriMaterial(_hdriMat);
		             renderQueue.SetHdriTexture(_hdriTexture);
		     
		             RenderQueueHelper::AddSceneComponent(renderQueue, scene.GetRoot(), true);
		             RenderQueueHelper::AddScenePhysicsDebug(renderQueue, &scene);
		             RenderQueueHelper::AddDebugLines(renderQueue, &scene);
		     
		             Renderer* renderer = Renderer::GetInstance();
		             renderer->SubmitRenderQueue(renderQueue);
		             */
	}

	/// @brief
	/// @param renderView
	void CameraComponent::SetupRenderView(RenderView& renderView)
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
	/// @param Size
	void CameraComponent::SetSize(float Size)
	{
		if (Size != _size)
		{
			_size = Size;
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
