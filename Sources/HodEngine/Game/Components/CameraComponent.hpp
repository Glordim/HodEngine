#pragma once
#include <HodEngine/HodEngine.hpp>

#include <vector>

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

#include "../Scene.hpp"

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;
		class Texture;
		class RenderQueue;
	}

	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API CameraComponent : public Component
		{
		public:

										CameraComponent() = default;
										CameraComponent(const CameraComponent&) = delete;
										CameraComponent(CameraComponent&&) = delete;
										~CameraComponent() override = default;

			void						operator=(const CameraComponent&) = delete;
			void						operator=(CameraComponent&&) = delete;

		public:

			void						Render(Scene& scene);

			const Matrix4&				GetProjectionMatrix();

			void						SetHdriMaterial(renderer::MaterialInstance* hdriMat, renderer::Texture* hdriTexture);

			void						PushToRenderQueue(renderer::RenderQueue& renderQueue) override;

		private:

			float						_aspect;
			float						_near;
			float						_far;
			bool						_perspective;
			bool						_dirtyFlag;

			renderer::MaterialInstance*	_hdriMat;
			renderer::Texture*			_hdriTexture;

			Matrix4						_projectionMatrix;
		};
	}
}
