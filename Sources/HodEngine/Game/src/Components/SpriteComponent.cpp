#include "SpriteComponent.h"

#include <HodEngine/Renderer/RHI/Material.h>
#include <HodEngine/Renderer/RenderQueue.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.h>
#include <HodEngine/Renderer/RHI/MaterialInstance.h>
#include <HodEngine/Renderer/Sprite.h>
#include <HodEngine/Renderer/SpriteAtlas.h>

#include "../Actor.h"

namespace hod
{
	namespace game
	{
		/*
		DESCRIBE_COMPONENT(SpriteComponent)
		{
			DescriptionProperty* spriteProperty = REGISTER_PROPERTY(_sprite);
			spriteProperty->SetSerializationName("sprite");
			spriteProperty->SetDisplayName("Sprite");
			spriteProperty->SetOption(Option::ResourceType, Sprite::_type);
		}
		*/

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
			renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(_sprite->GetVertices().data(), _sprite->GetVertices().size(), sizeof(renderer::P2fT2f), _sprite->GetIndices().data(), _sprite->GetIndices().size(), GetActor()->GetComponent<SceneComponent>()->GetModelMatrix(), _materialInstance));
		}
	}
}
