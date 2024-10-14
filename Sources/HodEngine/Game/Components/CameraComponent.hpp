#pragma once
#include "HodEngine/Game/Export.hpp"

#include <vector>

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

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
		REFLECTED_CLASS(CameraComponent, Component, HOD_GAME_API)

	public:

		enum class Projection : uint8_t
		{
			Perpective,
			Orthographic
		};

	public:

									CameraComponent() = default;
									CameraComponent(const CameraComponent&) = delete;
									CameraComponent(CameraComponent&&) = delete;
									~CameraComponent() override = default;

		void						operator=(const CameraComponent&) = delete;
		void						operator=(CameraComponent&&) = delete;

	public:

		void						Render(Scene& scene);

		void						SetProjection(Projection projection);
		Projection					GetProjection() const;

		const Matrix4&				GetProjectionMatrix();

		void						PushToRenderQueue(renderer::RenderQueue& renderQueue); // todo ?

	private:

		Projection					_projection = Projection::Perpective;
		float						_size = 5.0f;
		float						_near = -1.0f;
		float						_far = 1.0f;
		float						_fov = 60.0f;

		bool						_dirtyFlag = true;

		Matrix4						_projectionMatrix;
	};
}
