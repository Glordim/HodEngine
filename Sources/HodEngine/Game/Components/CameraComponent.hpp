#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Game/Component.hpp"

#include "../Scene.hpp"

namespace hod::renderer
{
	class MaterialInstance;
	class Texture;
	class RenderQueue;
}

namespace hod::game
{
	/// @brief
	class HOD_GAME_API CameraComponent : public Component
	{
		REFLECTED_CLASS(CameraComponent, Component)

	public:
		enum class Projection : uint8_t
		{
			Perpective,
			Orthographic
		};
		REFLECTED_ENUM(HOD_GAME_API, Projection);

		static CameraComponent* _main;

	public:
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = delete;
		CameraComponent(CameraComponent&&) = delete;
		~CameraComponent() override = default;

		void operator=(const CameraComponent&) = delete;
		void operator=(CameraComponent&&) = delete;

	public:
		void OnAwake() override;
		void OnDestruct() override;

		void Render(Scene& scene);

		void       SetProjection(Projection projection);
		Projection GetProjection() const;

		const Matrix4& GetProjectionMatrix();

		void SetupRenderView(renderer::RenderView& renderView);

		void  SetSize(float size);
		float GetSize() const;

		void SetAspect(float aspect);

	private:
		Projection _projection = Projection::Orthographic;
		float      _size = 5.0f;
		float      _near = -1.0f;
		float      _far = 1.0f;
		float      _fov = 60.0f;
		float      _aspect = 1.0f;

		bool _dirtyFlag = true;

		Matrix4 _projectionMatrix;
	};
}
