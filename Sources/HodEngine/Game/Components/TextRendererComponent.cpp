#include "HodEngine/Game/Components/TextRendererComponent.hpp"
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
		DESCRIBE_REFLECTED_CLASS(TextRendererComponent, RendererComponent)
		{
			AddPropertyT(this, &TextRendererComponent::_fontResource, "_fontResource", &TextRendererComponent::SetFont);
			//ADD_PROPERTY(TextureRendererComponent, _textureResource);
			//ADD_PROPERTY(SpriteRendererComponent, _materialInstance);
		}

		TextRendererComponent::TextRendererComponent()
		{
			const renderer::Material* material = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
			_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		TextRendererComponent::~TextRendererComponent()
		{
		}

		/// @brief 
		void TextRendererComponent::OnConstruct()
		{
			SetFont(_fontResource);
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
		void TextRendererComponent::SetMaterialInstance(renderer::MaterialInstance* materialInstance)
		{
			_materialInstance = materialInstance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::MaterialInstance* TextRendererComponent::GetMaterialInstance() const
		{
			return _materialInstance;
		}

		static std::array<renderer::P2fT2f, 4> _vertices = {
			renderer::P2fT2f(-0.5f, 0.5f, 0, 0),
			renderer::P2fT2f(0.5f, 0.5f, 1, 0),
			renderer::P2fT2f(0.5f, -0.5f, 1, 1),
			renderer::P2fT2f(-0.5f, -0.5f, 0, 1),
		};

		static std::array<uint16_t, 3*2> _indices = {
			0, 1, 2,
			0, 2, 3,
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void TextRendererComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			std::shared_ptr<Entity> entity = GetEntity();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(_vertices.data(), (uint32_t)_vertices.size(), sizeof(renderer::P2fT2f), _indices.data(), (uint32_t)_indices.size(), node2dComponent->GetWorldMatrix(), _materialInstance));
				}
			}
		}

		/// @brief 
		/// @param renderQueue 
		void TextRendererComponent::PushPickingToRenderQueue(renderer::RenderQueue& renderQueue, const Color& colorId)
		{
			std::shared_ptr<Entity> entity = GetEntity();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					std::array<renderer::P2fC4f, 4> vertices = {
						renderer::P2fC4f(-0.5f, 0.5f, colorId.r, colorId.g, colorId.b, colorId.a),
						renderer::P2fC4f(0.5f, 0.5f, colorId.r, colorId.g, colorId.b, colorId.a),
						renderer::P2fC4f(0.5f, -0.5f, colorId.r, colorId.g, colorId.b, colorId.a),
						renderer::P2fC4f(-0.5f, -0.5f, colorId.r, colorId.g, colorId.b, colorId.a),
					};

					const renderer::Material* pickingMaterial = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Fill_Triangle);
					renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(vertices.data(), (uint32_t)vertices.size(), sizeof(renderer::P2fC4f), _indices.data(), (uint32_t)_indices.size(), node2dComponent->GetWorldMatrix(), pickingMaterial->GetDefaultInstance()));
				}
			}
		}

		/// @brief 
		/// @param texture 
		void TextRendererComponent::SetFont(const WeakResource<renderer::FontResource>& font)
		{
			_fontResource = font;
			std::shared_ptr<renderer::FontResource> textureResourceLock = _fontResource.Lock();
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
