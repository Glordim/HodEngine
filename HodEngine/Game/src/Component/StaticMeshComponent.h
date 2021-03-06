#pragma once

#include "RendererComponent.h"

namespace HOD
{
	namespace RENDERER
	{
		class Mesh;
		class MaterialInstance;
	}

	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class StaticMeshComponent : public RendererComponent
		{
		public:

										StaticMeshComponent(Actor* actor);
										StaticMeshComponent(const StaticMeshComponent&) = delete;
										StaticMeshComponent(StaticMeshComponent&&) = delete;
										~StaticMeshComponent() override;

			void						operator=(const StaticMeshComponent&) = delete;
			void						operator=(StaticMeshComponent&&) = delete;

		public:

			void						DrawImGui() override;
			const char*					GetType() const override;

		public:
			RENDERER::Mesh*				GetMesh() const;
			void						SetMesh(RENDERER::Mesh* mesh);

			RENDERER::MaterialInstance*	GetMaterialInstance() const;
			void						SetMaterialInstance(RENDERER::MaterialInstance* materialInstance);

			bool						IsDebugTangentEnabled() const;
			void						EnableDebugTangent(bool enable);

		private:

			RENDERER::Mesh*				_mesh;
			RENDERER::MaterialInstance*	_materialInstance;

			bool						_debugTangent;
		};
	}
}
