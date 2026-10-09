#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/ImageNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>

#include "HodEngine/Core/StaticArray.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(ImageNode, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &ImageNode::_texture, "Texture", &ImageNode::SetTexture);
		AddPropertyT(reflectionDescriptor, &ImageNode::_color, "Color", &ImageNode::SetColor);
	}

	/// @brief
	/// @return
	const WeakResource<TextureResource>& ImageNode::GetTexture() const
	{
		return _texture;
	}

	/// @brief
	/// @param texture
	void ImageNode::SetTexture(const WeakResource<TextureResource>& texture)
	{
		if (_texture != texture)
		{
			_texture = texture;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Color& ImageNode::GetColor() const
	{
		return _color;
	}

	/// @brief
	/// @param color
	void ImageNode::SetColor(const Color& color)
	{
		if (_color != color)
		{
			_color = color;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @param drawContext
	void ImageNode::DrawSelf(DrawContext& drawContext)
	{
		// Without a (loaded) texture the node is a plain quad of its color, so a fresh Image is visible
		// right away and doubles as a colored rectangle.
		std::shared_ptr<TextureResource> textureResource = _texture.Lock();
		Texture*                         texture = textureResource != nullptr ? textureResource->GetTexture() : nullptr;

		const Vector2& size = GetSize();

		const StaticArray<Vector2, 4> vertices = {
			Vector2(-0.5f * size.GetX(), 0.5f * size.GetY()),
			Vector2(0.5f * size.GetX(), 0.5f * size.GetY()),
			Vector2(0.5f * size.GetX(), -0.5f * size.GetY()),
			Vector2(-0.5f * size.GetX(), -0.5f * size.GetY()),
		};
		static const StaticArray<Vector2, 4> uvs = {
			Vector2(0.0f, 0.0f),
			Vector2(1.0f, 0.0f),
			Vector2(1.0f, 1.0f),
			Vector2(0.0f, 1.0f),
		};
		static const StaticArray<uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};

		MaterialManager::BuiltinMaterial builtinMaterial =
			texture != nullptr ? MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color : MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle;

		MaterialInstance* materialInstance = MaterialInstance::Create(MaterialManager::GetInstance()->GetBuiltinMaterial(builtinMaterial));
		if (texture != nullptr)
		{
			materialInstance->SetTexture("image", texture);
		}
		materialInstance->SetVec4("ubo.color", Vector4(_color.r, _color.g, _color.b, _color.a));

		drawContext.PushMesh(vertices.Data(), texture != nullptr ? uvs.Data() : nullptr, (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), ComputeCanvasMatrix(),
		                     materialInstance);
	}
}
