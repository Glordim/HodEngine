#pragma once

#include "RendererComponent.h"

namespace HOD
{
	class Mesh;
	class MaterialInstance;

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

			void				operator=(const StaticMeshComponent&) = delete;
			void				operator=(StaticMeshComponent&&) = delete;

		public:

			void				DrawImGui() override;
			const char*			GetType() const override;

		public:
			Mesh*				GetMesh() const;
			void				SetMesh(Mesh* mesh);

			MaterialInstance*	GetMaterialInstance() const;
			void				SetMaterialInstance(MaterialInstance* materialInstance);

			bool				IsDebugTangentEnabled() const;
			void				EnableDebugTangent(bool enable);

		private:

			Mesh*				_mesh;
			MaterialInstance*	_materialInstance;

			bool				_debugTangent;
		};
	}
}
