#include "SpriteComponent.h"

#include <Renderer/src/RHI/Material.h>
#include <Renderer/src/RHI/Mesh.h>
#include <Renderer/src/RenderQueue.h>
#include <Renderer/src/RenderCommand/RenderCommandMesh.h>
#include <Renderer/src/RHI/MaterialInstance.h>
#include <Renderer/src/Sprite.h>
#include <Renderer/src/SpriteAtlas.h>

#include "../Actor.h"

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SpriteComponent::SpriteComponent(Actor* actor) : RendererComponent(actor)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SpriteComponent::~SpriteComponent()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::DrawImGui()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* SpriteComponent::GetType() const
		{
			return "Sprite";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::SetSprite(const RENDERER::Sprite* sprite)
		{
			_sprite = sprite;

			_materialInstance->SetTexture("textureSampler", *_sprite->GetSpriteAtlas()->GetTexture());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const RENDERER::Sprite* SpriteComponent::GetSprite() const
		{
			return _sprite;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::SetMaterialInstance(RENDERER::MaterialInstance* materialInstance)
		{
			_materialInstance = materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RENDERER::MaterialInstance* SpriteComponent::GetMaterialInstance() const
		{
			return _materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::PushToRenderQueue(RENDERER::RenderQueue& renderQueue)
		{
			/*
			std::vector<RENDERER::P2fT2f> vertices(6);
			vertices[0]._position[0] = -0.5f;
			vertices[0]._position[1] = -0.5f;
			vertices[0]._textCoord[0] = 0.0f;
			vertices[0]._textCoord[1] = 1.0f;
			vertices[1]._position[0] = 0.5f;
			vertices[1]._position[1] = -0.5f;
			vertices[1]._textCoord[0] = 1.0f;
			vertices[1]._textCoord[1] = 1.0f;
			vertices[2]._position[0] = 0.5f;
			vertices[2]._position[1] = 0.5f;
			vertices[2]._textCoord[0] = 1.0f;
			vertices[2]._textCoord[1] = 0.0f;
			*/
			renderQueue.PushRenderCommand(new RENDERER::RenderCommandMesh(_sprite->GetVertices().data(), _sprite->GetVertices().size(), _sprite->GetIndices().data(), _sprite->GetIndices().size(), GetActor()->GetComponent<SceneComponent>()->GetModelMatrix(), _materialInstance));
		}
	}
}
