#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/SpriteRendererComponent.hpp"

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
		DESCRIBE_REFLECTED_CLASS(SpriteRendererComponent, reflectionDescriptor)
		{
			AddPropertyT(reflectionDescriptor, &SpriteRendererComponent::_sprite, "_sprite");
			//ADD_PROPERTY(SpriteRendererComponent, _materialInstance);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SpriteRendererComponent::~SpriteRendererComponent()
		{

		}

		/*
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteRendererComponent::SetSprite(std::shared_ptr<renderer::Sprite> sprite)
		{
			_sprite = sprite;

			_materialInstance->SetTexture("textureSampler", _sprite->GetSpriteAtlas()->GetTexture());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const renderer::Sprite* SpriteRendererComponent::GetSprite() const
		{
			return _sprite;
		}
		*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteRendererComponent::SetMaterialInstance(renderer::MaterialInstance* materialInstance)
		{
			_materialInstance = materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::MaterialInstance* SpriteRendererComponent::GetMaterialInstance() const
		{
			return _materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SpriteRendererComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
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

		Rect SpriteRendererComponent::GetBoundingBox() const
		{
			return Rect();
		}
	}
}
