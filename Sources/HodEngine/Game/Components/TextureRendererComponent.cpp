#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/TextureRendererComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Sprite.hpp>
#include <HodEngine/Renderer/SpriteAtlas.hpp>
#include <HodEngine/Renderer/P2fT2f.hpp>
#include <HodEngine/Renderer/P2fC4f.hpp>
#include <HodEngine/Core/Color.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(TextureRendererComponent, RendererComponent)
		{
			AddPropertyT(this, &TextureRendererComponent::_textureResource, "_textureResource", &TextureRendererComponent::SetTexture);
			//ADD_PROPERTY(TextureRendererComponent, _textureResource);
			//ADD_PROPERTY(SpriteRendererComponent, _materialInstance);
		}

		TextureRendererComponent::TextureRendererComponent()
		{
			const renderer::Material* material = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
			_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		TextureRendererComponent::~TextureRendererComponent()
		{
			delete _materialInstance;
		}

		/// @brief 
		void TextureRendererComponent::OnConstruct()
		{
			SetTexture(_textureResource);
		}

		/*
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void TextureRendererComponent::SetSprite(std::shared_ptr<renderer::Sprite> sprite)
		{
			_sprite = sprite;

			_materialInstance->SetTexture("textureSampler", _sprite->GetSpriteAtlas()->GetTexture());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const renderer::Sprite* TextureRendererComponent::GetSprite() const
		{
			return _sprite;
		}
		*/

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void TextureRendererComponent::SetMaterialInstance(renderer::MaterialInstance* materialInstance)
		{
			_materialInstance = materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::MaterialInstance* TextureRendererComponent::GetMaterialInstance() const
		{
			return _materialInstance;
		}

		static std::array<Vector2, 4> _vertices = {
			Vector2(-0.5f, 0.5f),
			Vector2(0.5f, 0.5f),
			Vector2(0.5f, -0.5f),
			Vector2(-0.5f, -0.5f),
		};

		static std::array<Vector2, 4> _uvs = {
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1),
		};

		static std::array<uint16_t, 3*2> _indices = {
			0, 1, 2,
			0, 2, 3,
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void TextureRendererComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			std::shared_ptr<Entity> entity = GetEntity();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(_vertices.data(), _uvs.data(), nullptr, (uint32_t)_vertices.size(), _indices.data(), (uint32_t)_indices.size(), node2dComponent->GetWorldMatrix(), _materialInstance, (uint32_t)entity->GetId()));
				}
			}
		}

		/// @brief 
		/// @param texture 
		void TextureRendererComponent::SetTexture(const WeakResource<renderer::TextureResource>& texture)
		{
			_textureResource = texture;
			std::shared_ptr<renderer::TextureResource> textureResourceLock = _textureResource.Lock();
			if (textureResourceLock != nullptr)
			{
				_materialInstance->SetTexture("image", textureResourceLock->GetTexture());
			}
			else
			{
				_materialInstance->SetTexture("image", nullptr);
			}
		}
	}
}
