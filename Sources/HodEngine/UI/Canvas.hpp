#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Game/Components/RendererComponent.hpp>

#include <HodEngine/Game/Components/CameraComponent.hpp>

namespace hod::inline game
{
	class Entity;
}

namespace hod::inline ui
{
	class Node;
	class Rebuildable;

	/// @brief
	class HOD_UI_API Canvas : public RendererComponent
	{
		REFLECTED_CLASS(Canvas, RendererComponent)

	public:
		enum class RenderMode
		{
			World,
			Camera,
		};
		REFLECTED_ENUM(HOD_UI_API, RenderMode);

		enum class ScaleMode
		{
			WidthHeight,
			Expand,
			Shrink,
			Fixed,
		};
		REFLECTED_ENUM(HOD_UI_API, ScaleMode);

	public:
		void OnEnable() override;
		void OnUpdate(float deltaTime) override;

		void       SetRenderMode(RenderMode renderMode);
		RenderMode GetRenderMode() const;

		void                   SetCamera(CameraComponent* camera);
		CameraComponent* GetCamera() const;

		ScaleMode GetScaleMode() const;
		void      SetScaleMode(ScaleMode scaleMode);

		float GetWidthHeightPreferredAxis() const;
		void  SetWidthHeightPreferredAxis(float widthHeightPreferredAxis);

		void RecomputeRootNodeSize(const Vector2& resolution);

		Rect GetBoundingBox() const override;
		void PushRenderCommand(renderer::RenderView& renderView) override;

		const Matrix4& GetRenderModeMatrix() const;

		void MarkForRebuild(Rebuildable* rebuildable);

		void DoRebuild();

	private:
		RenderMode                     _renderMode = RenderMode::Camera;
		WeakPtr<CameraComponent> _camera;

		WeakPtr<Node> _rootNode;

		ScaleMode _scaleMode = ScaleMode::WidthHeight;
		float     _widthHeightPreferredAxis = 0.5f;

		float _scaleFactor = 1.0f;

		Matrix4 _renderModeMatrix;

		Vector<WeakPtr<Rebuildable>> _markedForRebuild;
	};
}
