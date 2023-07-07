#pragma once

#include <vector>

#include "HodEngine/Game/Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include "../Scene.h"

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
		class PointLightComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class CameraComponent : public Component
		{
		public:

										CameraComponent() = default;
										CameraComponent(const CameraComponent&) = delete;
										CameraComponent(CameraComponent&&) = delete;
										~CameraComponent() override = default;

			void						operator=(const CameraComponent&) = delete;
			void						operator=(CameraComponent&&) = delete;

		public:

			const char*					GetType() const override;

			void						Render(Scene& scene);

			const glm::mat4&			GetProjectionMatrix();

			void						SetHdriMaterial(renderer::MaterialInstance* hdriMat, renderer::Texture* hdriTexture);

			void						PushToRenderQueue(renderer::RenderQueue* renderQueue = nullptr);

		private:

			float						_fov;
			float						_aspect;
			float						_near;
			float						_far;
			bool						_perspective;
			bool						_dirtyFlag;

			renderer::MaterialInstance*	_hdriMat;
			renderer::Texture*			_hdriTexture;

			glm::mat4					_projectionMatrix;
		};
	}
}
