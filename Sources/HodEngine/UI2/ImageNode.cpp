#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/ImageNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Texture.hpp>

#include "HodEngine/Core/StaticArray.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(ImageNode, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_IMAGE);

		AddPropertyT(reflectionDescriptor, &ImageNode::_texture, "Texture", &ImageNode::SetTexture);
		AddPropertyT(reflectionDescriptor, &ImageNode::_color, "Color", &ImageNode::SetColor);
		AddPropertyT(reflectionDescriptor, &ImageNode::_slices, "Slices", &ImageNode::SetSlices);
		AddPropertyT(reflectionDescriptor, &ImageNode::_sliceScale, "SliceScale", &ImageNode::SetSliceScale);
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
			MarkMeasureAsDirty(); // the natural size is the texture's
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
	/// @return
	const RectOffsets& ImageNode::GetSlices() const
	{
		return _slices;
	}

	/// @brief
	/// @param slices negative values are clamped to 0
	void ImageNode::SetSlices(const RectOffsets& slices)
	{
		RectOffsets clamped;
		clamped._left = Max(slices._left, 0.0f);
		clamped._top = Max(slices._top, 0.0f);
		clamped._right = Max(slices._right, 0.0f);
		clamped._bottom = Max(slices._bottom, 0.0f);
		if (_slices != clamped)
		{
			_slices = clamped;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	float ImageNode::GetSliceScale() const
	{
		return _sliceScale;
	}

	/// @brief
	/// @param sliceScale negative values are clamped to 0
	void ImageNode::SetSliceScale(float sliceScale)
	{
		sliceScale = Max(sliceScale, 0.0f);
		if (_sliceScale != sliceScale)
		{
			_sliceScale = sliceScale;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief The texture's size once it is loaded, none before that.
	/// @param availableSize
	/// @return
	Vector2 ImageNode::MeasureContent(const Vector2& availableSize) const
	{
		(void)availableSize;

		std::shared_ptr<TextureResource> textureResource = _texture.Lock();
		Texture*                         texture = textureResource != nullptr ? textureResource->GetTexture() : nullptr;
		if (texture == nullptr)
		{
			return Vector2::Zero;
		}
		return Vector2((float)texture->GetWidth(), (float)texture->GetHeight());
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
		const float    halfWidth = 0.5f * size.GetX();
		const float    halfHeight = 0.5f * size.GetY();

		// A 4x4 grid of vertices, row by row from the top left: its inner lines are where the slices are cut. Not sliced, the
		// inner lines are on the edges and its cells empty: the same as one quad, then drawn as one
		StaticArray<float, 4> xs = {-halfWidth, -halfWidth, halfWidth, halfWidth};
		StaticArray<float, 4> ys = {halfHeight, halfHeight, -halfHeight, -halfHeight};
		StaticArray<float, 4> us = {0.0f, 0.0f, 1.0f, 1.0f};
		StaticArray<float, 4> vs = {0.0f, 0.0f, 1.0f, 1.0f};

		const bool sliced = texture != nullptr && (_slices._left > 0.0f || _slices._top > 0.0f || _slices._right > 0.0f || _slices._bottom > 0.0f);
		if (sliced)
		{
			const float textureWidth = (float)texture->GetWidth();
			const float textureHeight = (float)texture->GetHeight();

			// Slices larger than the texture are cut to it
			const float left = Min(_slices._left, textureWidth);
			const float right = Min(_slices._right, textureWidth - left);
			const float top = Min(_slices._top, textureHeight);
			const float bottom = Min(_slices._bottom, textureHeight - top);

			us[1] = left / textureWidth;
			us[2] = 1.0f - right / textureWidth;
			vs[1] = top / textureHeight;
			vs[2] = 1.0f - bottom / textureHeight;

			// In a node smaller than its corners, the corners shrink to fit, keeping their proportions on each axis
			float leftSize = left * _sliceScale;
			float rightSize = right * _sliceScale;
			if (leftSize + rightSize > size.GetX() && leftSize + rightSize > 0.0f)
			{
				const float fit = size.GetX() / (leftSize + rightSize);
				leftSize *= fit;
				rightSize *= fit;
			}
			float topSize = top * _sliceScale;
			float bottomSize = bottom * _sliceScale;
			if (topSize + bottomSize > size.GetY() && topSize + bottomSize > 0.0f)
			{
				const float fit = size.GetY() / (topSize + bottomSize);
				topSize *= fit;
				bottomSize *= fit;
			}

			xs[1] = -halfWidth + leftSize;
			xs[2] = halfWidth - rightSize;
			ys[1] = halfHeight - topSize;
			ys[2] = -halfHeight + bottomSize;
		}

		StaticArray<Vector2, 16> vertices;
		StaticArray<Vector2, 16> uvs;
		for (uint32_t row = 0; row < 4; ++row)
		{
			for (uint32_t column = 0; column < 4; ++column)
			{
				vertices[row * 4 + column] = Vector2(xs[column], ys[row]);
				uvs[row * 4 + column] = Vector2(us[column], vs[row]);
			}
		}

		// The 9 cells; one quad, the corners of the grid, when not sliced
		static const StaticArray<uint16_t, 54> slicedIndices = []()
		{
			StaticArray<uint16_t, 54> indices;
			uint32_t                  index = 0;
			for (uint16_t row = 0; row < 3; ++row)
			{
				for (uint16_t column = 0; column < 3; ++column)
				{
					const uint16_t topLeft = row * 4 + column;
					indices[index++] = topLeft;
					indices[index++] = topLeft + 1;
					indices[index++] = topLeft + 5;
					indices[index++] = topLeft;
					indices[index++] = topLeft + 5;
					indices[index++] = topLeft + 4;
				}
			}
			return indices;
		}();
		static const StaticArray<uint16_t, 6> quadIndices = {0, 3, 15, 0, 15, 12};

		const uint16_t* indices = sliced ? slicedIndices.Data() : quadIndices.Data();
		const uint32_t  indexCount = sliced ? (uint32_t)slicedIndices.Size() : (uint32_t)quadIndices.Size();

		BuiltinMaterialManager::BuiltinMaterial builtinMaterial =
			texture != nullptr ? BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color : BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle;

		MaterialInstance* materialInstance = MaterialInstance::Create(BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(builtinMaterial));
		if (texture != nullptr)
		{
			materialInstance->SetTexture("image", texture);
		}
		materialInstance->SetVec4("ubo.color", Vector4(_color.r, _color.g, _color.b, _color.a));

		drawContext.PushMesh(vertices.Data(), texture != nullptr ? uvs.Data() : nullptr, (uint32_t)vertices.Size(), indices, indexCount, ComputeCanvasMatrix(), materialInstance);
	}
}
