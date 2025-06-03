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

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(TextureRendererComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &TextureRendererComponent::_texture, "_texture", &TextureRendererComponent::SetTexture);
		AddPropertyT(reflectionDescriptor, &TextureRendererComponent::_material, "_material", &TextureRendererComponent::SetMaterialInstanceResource);
		AddPropertyT(reflectionDescriptor, &TextureRendererComponent::_pixelPerUnit, "_pixelPerUnit");
		AddPropertyT(reflectionDescriptor, &TextureRendererComponent::_color, "_color", &TextureRendererComponent::SetColor);
	}

	TextureRendererComponent::TextureRendererComponent()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	TextureRendererComponent::~TextureRendererComponent()
	{
		delete _builtinMaterialInstance;
	}

	/// @brief 
	void TextureRendererComponent::OnConstruct()
	{
		std::shared_ptr<renderer::MaterialInstanceResource> materialInstanceResource = _material.Lock();
		if (materialInstanceResource != nullptr)
		{
			_materialInstance = materialInstanceResource->GetMaterialInstance();
		}
		RefreshMaterialInstance();
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

	/// @brief 
	/// @param materialInstance 
	void TextureRendererComponent::SetMaterialInstanceResource(const WeakResource<renderer::MaterialInstanceResource>& weakMaterialInstanceResource)
	{
		_material = weakMaterialInstanceResource;
		std::shared_ptr<renderer::MaterialInstanceResource> materialInstanceResource = _material.Lock();
		if (materialInstanceResource != nullptr)
		{
			_materialInstance = materialInstanceResource->GetMaterialInstance();
			RefreshMaterialInstance();
		}
		else
		{
			_materialInstance = nullptr;
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	renderer::MaterialInstance* TextureRendererComponent::GetMaterialInstance() const
	{
		return _materialInstance;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void TextureRendererComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
	{
		Entity* entity = GetOwner();
		if (entity != nullptr)
		{
			Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				Rect bb = GetBoundingBox();

				std::array<Vector2, 4> vertices = {
					Vector2(-0.5f * bb._size.GetX(), 0.5f * bb._size.GetY()),
					Vector2(0.5f * bb._size.GetX(), 0.5f * bb._size.GetY()),
					Vector2(0.5f * bb._size.GetX(), -0.5f * bb._size.GetY()),
					Vector2(-0.5f * bb._size.GetX(), -0.5f * bb._size.GetY()),
				};

				static std::array<Vector2, 4> uvs = {
					Vector2(0, 0),
					Vector2(1, 0),
					Vector2(1, 1),
					Vector2(0, 1),
				};

				static std::array<uint16_t, 3*2> indices = {
					0, 1, 2,
					0, 2, 3,
				};

				if (_materialInstance == nullptr)
				{
					const renderer::Material* material = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
					_builtinMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
					_materialInstance = _builtinMaterialInstance;
					RefreshMaterialInstance();
				}

				renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(vertices.data(), uvs.data(), nullptr, (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), node2dComponent->GetWorldMatrix(), _materialInstance, node2dComponent->GetZOrder().GetValue(), (uint32_t)entity->GetInstanceId()));
			}
		}
	}

	/// @brief 
	/// @param texture 
	void TextureRendererComponent::SetTexture(const WeakResource<renderer::TextureResource>& texture)
	{
		_texture = texture;
		RefreshMaterialInstance();
	}

	/// @brief 
	void TextureRendererComponent::RefreshMaterialInstance()
	{
		if (_materialInstance != nullptr)
		{
			std::shared_ptr<renderer::TextureResource> textureResourceLock = _texture.Lock();
			if (textureResourceLock != nullptr)
			{
				_materialInstance->SetTexture("image", textureResourceLock->GetTexture());
			}
			else
			{
				_materialInstance->SetTexture("image", nullptr);
			}

			Vector4 vec4Color;
			vec4Color.SetX(_color.r);
			vec4Color.SetY(_color.g);
			vec4Color.SetZ(_color.b);
			vec4Color.SetW(_color.a);
			_materialInstance->SetVec4("UBO.color", vec4Color);
		}
	}

	/// @brief 
	/// @return 
	Rect TextureRendererComponent::GetBoundingBox() const
	{
		float width = _pixelPerUnit;
		float height = _pixelPerUnit;

		std::shared_ptr<renderer::TextureResource> textureResourceLock = _texture.Lock();
		if (textureResourceLock != nullptr)
		{
			width = (float)textureResourceLock->GetTexture()->GetWidth();
			height = (float)textureResourceLock->GetTexture()->GetHeight();
		}

		Rect bb;
		bb._position = Vector2::Zero;
		bb._size = Vector2(width, height) / _pixelPerUnit;
		return bb;
	}

	/// @brief 
	/// @param color 
	void TextureRendererComponent::SetColor(const Color& color)
	{
		_color = color;
		RefreshMaterialInstance();
	}

	/// @brief 
	/// @return 
	const Color& TextureRendererComponent::GetColor() const
	{
		return _color;
	}
}
