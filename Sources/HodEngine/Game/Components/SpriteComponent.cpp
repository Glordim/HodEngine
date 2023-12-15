#include "HodEngine/Game/Components/SpriteComponent.hpp"

#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Sprite.hpp>
#include <HodEngine/Renderer/SpriteAtlas.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(SpriteComponent, RendererComponent)
		{
			//ADD_PROPERTY(SpriteComponent, _sprite);
			//ADD_PROPERTY(SpriteComponent, _materialInstance);
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
		const char* SpriteComponent::GetType() const
		{
			return "Sprite";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::SetSprite(const renderer::Sprite* sprite)
		{
			_sprite = sprite;

			_materialInstance->SetTexture("textureSampler", _sprite->GetSpriteAtlas()->GetTexture());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const renderer::Sprite* SpriteComponent::GetSprite() const
		{
			return _sprite;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::SetMaterialInstance(renderer::MaterialInstance* materialInstance)
		{
			_materialInstance = materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::MaterialInstance* SpriteComponent::GetMaterialInstance() const
		{
			return _materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			/*
			std::vector<renderer::P2fT2f> vertices(6);
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
			//renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(_sprite->GetVertices().data(), _sprite->GetVertices().size(), sizeof(renderer::P2fT2f), _sprite->GetIndices().data(), _sprite->GetIndices().size(), GetActor()->GetComponent<SceneComponent>()->GetModelMatrix(), _materialInstance));
		}
	}
}
