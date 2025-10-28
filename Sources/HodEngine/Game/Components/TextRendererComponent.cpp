#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Components/TextRendererComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Renderer/Font/Font.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/P2fC4f.hpp>
#include <HodEngine/Renderer/P2fT2f.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Sprite.hpp>
#include <HodEngine/Renderer/SpriteAtlas.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(TextRendererComponent, reflectionDescriptor)
		{
			AddPropertyT(reflectionDescriptor, &TextRendererComponent::_fontResource, "_fontResource", &TextRendererComponent::SetFont);
			AddPropertyT(reflectionDescriptor, &TextRendererComponent::_text, "_text" /*, &TextRendererComponent::SetText*/);
			// ADD_PROPERTY(TextureRendererComponent, _textureResource);
			// ADD_PROPERTY(SpriteRendererComponent, _materialInstance);
		}

		/// @brief
		TextRendererComponent::TextRendererComponent()
		{
			const renderer::Material* material = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
			_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
		}

		/// @brief
		TextRendererComponent::~TextRendererComponent()
		{
			DefaultAllocator::GetInstance().Delete(_materialInstance);
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

		Rect TextRendererComponent::GetBoundingBox() const
		{
			return Rect();
		}

		//-----------------------------------------------------------------------------
		//! @brief
		//-----------------------------------------------------------------------------
		void TextRendererComponent::PushRenderCommand(renderer::RenderView& renderView)
		{
			(void)renderView; // TODO

			if (_text.Empty() || _fontResource.Lock() == nullptr)
			{
				return;
			}

			Entity* entity = GetOwner();
			if (entity != nullptr)
			{
				Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					/*
					renderer::Font* font = _fontResource.Lock()->GetFont();

					float atlasWidth = (float)font->GetTexture()->GetWidth();
					float atlasHeight = (float)font->GetTexture()->GetHeight();

					Vector<Vector2> positions;
					positions.reserve(_text.Size() * 4);

					Vector<Vector2> uvs;
					uvs.reserve(_text.Size() * 4);

					Vector<uint16_t> indices;
					indices.reserve(_text.Size() * 6);

					float offset = 0;

					const char* str = _text.c_str();
					while (*str != '\0')
					{
					    const renderer::Font::GlyphInfo& kerning = font->GetGlyphInfo(*str);

					    uint16_t vertexCount = (uint16_t)positions.Size();

					    positions.emplace_back(offset, (float)kerning._baseline * 0.5f  * -0.01f + (float)kerning._size.GetY() * 0.5f * 0.01f);
					    uvs.emplace_back((float)kerning._offsetX / atlasWidth, (float)kerning._offsetY / atlasHeight);

					    positions.emplace_back(offset + (float)kerning._size.GetX() * 0.01f, (float)kerning._baseline * 0.5f  * -0.01f + (float)kerning._size.GetY() * 0.5f *
					0.01f); uvs.emplace_back((float)(kerning._offsetX + kerning._size.GetX()) / atlasWidth, (float)kerning._offsetY / atlasHeight);

					    positions.emplace_back(offset + (float)kerning._size.GetX() * 0.01f, (float)kerning._baseline * 0.5f  * -0.01f + (float)kerning._size.GetY() * -0.5f *
					0.01f); uvs.emplace_back((float)(kerning._offsetX + kerning._size.GetX()) / atlasWidth, (float)(kerning._offsetY + kerning._size.GetY()) / atlasHeight);

					    positions.emplace_back(offset, (float)kerning._baseline * 0.5f * -0.01f + (float)kerning._size.GetY() * -0.5f * 0.01f);
					    uvs.emplace_back((float)kerning._offsetX / atlasWidth, (float)(kerning._offsetY + kerning._size.GetY()) / atlasHeight);

					    indices.push_back(vertexCount);
					    indices.push_back(vertexCount + 1);
					    indices.push_back(vertexCount + 2);

					    indices.push_back(vertexCount);
					    indices.push_back(vertexCount + 2);
					    indices.push_back(vertexCount + 3);

					    offset += (kerning._size.GetX() + 2) * 0.01f;

					    ++str;
					}

					if (positions.empty() == false)
					{
					    renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(positions.Data(), uvs.Data(), nullptr,
					(uint32_t)positions.Size(), indices.Data(), (uint32_t)indices.Size(), node2dComponent->GetWorldMatrix(), _materialInstance,
					node2dComponent->GetZOrder().GetValue(), (uint32_t)entity->GetInstanceId()));
					}
					*/
				}
			}
		}

		/// @brief
		/// @param texture
		void TextRendererComponent::SetFont(const WeakResource<renderer::FontResource>& font)
		{
			_fontResource = font;

			renderer::Texture*                      texture = nullptr;
			std::shared_ptr<renderer::FontResource> fontResource = _fontResource.Lock();
			if (fontResource != nullptr)
			{
				renderer::Font* font = fontResource->GetFont();
				if (font != nullptr)
				{
					texture = font->GetTexture();
				}
			}
			else
			{
			}
			_materialInstance->SetTexture("image", texture);
		}
	}
}
