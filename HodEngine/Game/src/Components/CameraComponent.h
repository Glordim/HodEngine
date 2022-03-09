#pragma once

#include <vector>

#include "../Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include "../Scene.h"

namespace HOD
{
	namespace RENDERER
	{
		class MaterialInstance;
		class Texture;
		class RenderQueue;
	}

	namespace GAME
	{
		class PointLightComponent;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class CameraComponent : public Component
		{
		public:

										CameraComponent(Actor* actor);
										CameraComponent(const CameraComponent&) = delete;
										CameraComponent(CameraComponent&&) = delete;
										~CameraComponent() override = default;

			void						operator=(const CameraComponent&) = delete;
			void						operator=(CameraComponent&&) = delete;

		public:

			void						DrawImGui() override;
			const char*					GetType() const override;

			void						Render(Scene& scene);

			const glm::mat4&			GetProjectionMatrix();

			void						SetHdriMaterial(RENDERER::MaterialInstance* hdriMat, RENDERER::Texture* hdriTexture);

			void						PushToRenderQueue(RENDERER::RenderQueue* renderQueue = nullptr);

		private:

			float						_fov;
			float						_aspect;
			float						_near;
			float						_far;
			bool						_perspective;
			bool						_dirtyFlag;

			RENDERER::MaterialInstance*	_hdriMat;
			RENDERER::Texture*			_hdriTexture;

			glm::mat4					_projectionMatrix;
		};
	}
}
