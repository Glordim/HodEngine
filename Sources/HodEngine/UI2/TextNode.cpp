#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/TextNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/Text/GlyphCache.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Sampler.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextShaper.hpp>
#include <HodEngine/UI2/LayoutParams.hpp>

#include <cmath>
#include <limits>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(TextNode::HorizontalAlignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(TextNode::HorizontalAlignment::Left, "Left");
		reflectionDescriptor.AddEnumValue(TextNode::HorizontalAlignment::Center, "Center");
		reflectionDescriptor.AddEnumValue(TextNode::HorizontalAlignment::Right, "Right");
	}

	DESCRIBE_REFLECTED_ENUM(TextNode::VerticalAlignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(TextNode::VerticalAlignment::Top, "Top");
		reflectionDescriptor.AddEnumValue(TextNode::VerticalAlignment::Middle, "Middle");
		reflectionDescriptor.AddEnumValue(TextNode::VerticalAlignment::Capline, "Capline");
		reflectionDescriptor.AddEnumValue(TextNode::VerticalAlignment::Bottom, "Bottom");
	}

	DESCRIBE_REFLECTED_ENUM(TextNode::Overflow, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(TextNode::Overflow::Overflow, "Overflow");
		reflectionDescriptor.AddEnumValue(TextNode::Overflow::Clip, "Clip");
		reflectionDescriptor.AddEnumValue(TextNode::Overflow::Ellipsis, "Ellipsis");
		reflectionDescriptor.AddEnumValue(TextNode::Overflow::Shrink, "Shrink");
	}

	DESCRIBE_REFLECTED_CLASS(TextNode, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_FORMAT_TEXT);

		AddPropertyT(reflectionDescriptor, &TextNode::_font, "Font", &TextNode::SetFont);
		AddPropertyT(reflectionDescriptor, &TextNode::_value, "Value", &TextNode::SetValue);
		AddPropertyT(reflectionDescriptor, &TextNode::_fontSize, "FontSize", &TextNode::SetFontSize);
		AddPropertyT(reflectionDescriptor, &TextNode::_fontWeight, "FontWeight", &TextNode::SetFontWeight);
		AddPropertyT(reflectionDescriptor, &TextNode::_italic, "Italic", &TextNode::SetItalic);
		AddPropertyT(reflectionDescriptor, &TextNode::_horizontalAlignment, "HorizontalAlignment", &TextNode::SetHorizontalAlignment);
		AddPropertyT(reflectionDescriptor, &TextNode::_verticalAlignment, "VerticalAlignment", &TextNode::SetVerticalAlignment);
		AddPropertyT(reflectionDescriptor, &TextNode::_alignByGeometry, "AlignByGeometry", &TextNode::SetAlignByGeometry);
		AddPropertyT(reflectionDescriptor, &TextNode::_overflow, "Overflow", &TextNode::SetOverflow);
		AddPropertyT(reflectionDescriptor, &TextNode::_minFontSize, "MinFontSize", &TextNode::SetMinFontSize);
		AddPropertyT(reflectionDescriptor, &TextNode::_color, "Color", &TextNode::SetColor);
	}

	/// @brief
	/// @return
	const WeakResource<FontResource>& TextNode::GetFont() const
	{
		return _font;
	}

	/// @brief
	/// @param font
	void TextNode::SetFont(const WeakResource<FontResource>& font)
	{
		if (_font != font)
		{
			_font = font;
			_layoutDirty = true;
			MarkMeasureAsDirty(); // the text bounds depend on the font
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const String& TextNode::GetValue() const
	{
		return _value;
	}

	/// @brief
	/// @param value
	void TextNode::SetValue(const String& value)
	{
		if (_value != value)
		{
			_value = value;
			_layoutDirty = true;
			MarkMeasureAsDirty(); // the text bounds depend on the text
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	float TextNode::GetFontSize() const
	{
		return _fontSize;
	}

	/// @brief
	/// @param fontSize
	void TextNode::SetFontSize(float fontSize)
	{
		if (_fontSize != fontSize)
		{
			_fontSize = fontSize;
			_layoutDirty = true;
			MarkMeasureAsDirty(); // the text bounds depend on the size
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	FontResource::Weight TextNode::GetFontWeight() const
	{
		return _fontWeight;
	}

	/// @brief
	/// @param fontWeight
	void TextNode::SetFontWeight(FontResource::Weight fontWeight)
	{
		if (_fontWeight != fontWeight)
		{
			_fontWeight = fontWeight;
			_layoutDirty = true;
			MarkMeasureAsDirty(); // another font maybe
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	bool TextNode::IsItalic() const
	{
		return _italic;
	}

	/// @brief
	/// @param italic
	void TextNode::SetItalic(bool italic)
	{
		if (_italic != italic)
		{
			_italic = italic;
			_layoutDirty = true;
			MarkMeasureAsDirty(); // another font maybe
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	TextNode::HorizontalAlignment TextNode::GetHorizontalAlignment() const
	{
		return _horizontalAlignment;
	}

	/// @brief
	/// @param horizontalAlignment
	void TextNode::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
	{
		if (_horizontalAlignment != horizontalAlignment)
		{
			_horizontalAlignment = horizontalAlignment;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	TextNode::VerticalAlignment TextNode::GetVerticalAlignment() const
	{
		return _verticalAlignment;
	}

	/// @brief
	/// @param verticalAlignment
	void TextNode::SetVerticalAlignment(VerticalAlignment verticalAlignment)
	{
		if (_verticalAlignment != verticalAlignment)
		{
			_verticalAlignment = verticalAlignment;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	bool TextNode::GetAlignByGeometry() const
	{
		return _alignByGeometry;
	}

	/// @brief
	/// @param alignByGeometry
	void TextNode::SetAlignByGeometry(bool alignByGeometry)
	{
		if (_alignByGeometry != alignByGeometry)
		{
			_alignByGeometry = alignByGeometry;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	TextNode::Overflow TextNode::GetOverflow() const
	{
		return _overflow;
	}

	/// @brief
	/// @param overflow
	void TextNode::SetOverflow(Overflow overflow)
	{
		if (_overflow != overflow)
		{
			_overflow = overflow;
			_fittedDirty = true;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	float TextNode::GetMinFontSize() const
	{
		return _minFontSize;
	}

	/// @brief
	/// @param minFontSize
	void TextNode::SetMinFontSize(float minFontSize)
	{
		if (_minFontSize != minFontSize)
		{
			_minFontSize = minFontSize;
			_fittedDirty = true;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Color& TextNode::GetColor() const
	{
		return _color;
	}

	/// @brief
	/// @param color
	void TextNode::SetColor(const Color& color)
	{
		if (_color != color)
		{
			_color = color;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief The font, its style and their fallbacks are followed: shaped again when one of them is reloaded (another face), drawn otherwise, added or removed
	/// @return
	const TextLayout* TextNode::UpdateLayout() const
	{
		const bool chainChanged = _fontChain.Update(_font.Lock(), _fontWeight, _italic);
		if (_fontChain.Empty() || _fontSize <= 0.0f)
		{
			return nullptr;
		}

		// The size is the one of the font drawn (the style): its fallbacks are drawn at it
		const float drawnFontSize = _fontChain.GetFonts()[0]->GetDrawnFontSize(_fontSize);
		if (_layoutDirty || chainChanged || _layoutFontSize != drawnFontSize)
		{
			if (Renderer::GetInstance()->GetTextShaper()->Shape(_value, _fontChain.GetFaces(), drawnFontSize, _layout) == false)
			{
				_layout.Clear();
			}
			_layoutFontSize = drawnFontSize;
			_layoutDirty = false;
			_fittedDirty = true;
		}
		return &_layout;
	}

	/// @brief
	/// @return
	bool TextNode::IsPixelArt() const
	{
		return _fontChain.Empty() == false && _fontChain.GetMeshFonts()[0]._mode == GlyphCache::Mode::Pixel;
	}

	/// @brief
	/// @return
	uint32_t TextNode::GetPixelSize() const
	{
		return _fontChain.Empty() ? 0 : _fontChain.GetMeshFonts()[0]._pixelSize;
	}

	/// @brief On the axes sized from the content the rect is the size of the text: it fits there whatever Overflow says
	/// @param size of the rect
	/// @return
	const TextLayout* TextNode::UpdateFittedLayout(const Vector2& size)
	{
		const TextLayout* layout = UpdateLayout();
		if (layout == nullptr)
		{
			return nullptr;
		}

		if (_fittedDirty == false && _fittedSize == size)
		{
			return &_fittedLayout;
		}
		_fittedDirty = false;
		_fittedSize = size;
		_meshDirty = true;

		_fittedLayout = *layout;
		_fittedFontSize = _layoutFontSize;
		if (_overflow != Overflow::Ellipsis && _overflow != Overflow::Shrink)
		{
			return &_fittedLayout;
		}

		const LayoutParams* layoutParams = GetLayoutParams();
		const bool          widthFromContent = layoutParams != nullptr && layoutParams->GetWidthMode() == LayoutParams::SizeMode::Content;
		const bool          heightFromContent = layoutParams != nullptr && layoutParams->GetHeightMode() == LayoutParams::SizeMode::Content;
		const float         maxWidth = widthFromContent ? std::numeric_limits<float>::max() : size.GetX();
		const float         maxHeight = heightFromContent ? std::numeric_limits<float>::max() : size.GetY();

		const FontResource&        fontResource = *_fontChain.GetFonts()[0];
		ArrayView<const FontFace*> faces = _fontChain.GetFaces();
		TextShaper*                textShaper = Renderer::GetInstance()->GetTextShaper();

		if (_overflow == Overflow::Shrink)
		{
			// The positions scale with the size: the size that fits is known from the layout at FontSize
			const Vector2 layoutSize = GetLayoutSize(*layout);
			float         scale = 1.0f;
			if (layoutSize.GetX() > maxWidth)
			{
				scale = Min(scale, maxWidth / layoutSize.GetX());
			}
			if (layoutSize.GetY() > maxHeight)
			{
				scale = Min(scale, maxHeight / layoutSize.GetY());
			}
			if (scale >= 1.0f)
			{
				return &_fittedLayout;
			}

			// A whole pixel size below the exact one (PixelArt: a whole multiple of its grid): it fits for sure (the exact one may be a hair wider
			// once rounded), and is rasterized as is. Only what does not fit even at MinFontSize gets an ellipsis
			const float step = IsPixelArt() ? (float)GetPixelSize() : 1.0f;
			const float minFontSize = Min(fontResource.GetDrawnFontSize(_minFontSize), _layoutFontSize);
			const float fittingFontSize = _layoutFontSize * scale;
			if (fittingFontSize >= minFontSize)
			{
				_fittedFontSize = Max(minFontSize, std::floor(fittingFontSize / step) * step);
				if (textShaper->Shape(_value, faces, _fittedFontSize, _fittedLayout) == false)
				{
					_fittedLayout.Clear();
				}
				return &_fittedLayout;
			}
			_fittedFontSize = minFontSize;
		}

		if (textShaper->ShapeEllipsized(_value, faces, _fittedFontSize, maxWidth, _fittedLayout) == false)
		{
			_fittedLayout.Clear();
		}
		return &_fittedLayout;
	}

	/// @brief
	/// @param layout
	/// @return
	Vector2 TextNode::GetLayoutSize(const TextLayout& layout)
	{
		return Vector2(layout._width, layout._ascender - layout._descender);
	}

	/// @brief The text's bounds once a font is set, none before that.
	/// @param availableSize
	/// @return
	Vector2 TextNode::MeasureContent(const Vector2& availableSize) const
	{
		if (UpdateLayout() == nullptr)
		{
			return Node::MeasureContent(availableSize);
		}

		return GetLayoutSize(_layout);
	}

	/// @brief
	/// @param drawContext
	void TextNode::DrawSelf(DrawContext& drawContext)
	{
		const Vector2&    size = GetSize();
		const TextLayout* layout = UpdateFittedLayout(size);
		if (layout == nullptr || layout->_glyphs.Empty())
		{
			return;
		}

		const Vector2 requiredSize = GetLayoutSize(*layout);

		// Built again too when the atlas of a font grew: the uvs of its glyphs changed
		if (_meshDirty || _mesh.IsOutdated())
		{
			_mesh.Build(*layout, _fontChain.GetMeshFonts(), _fittedFontSize, Vector2(0.0f, 0.0f));
			_meshDirty = false;
		}

		// What is aligned, from the start of the baseline: the line, or the ink of the text
		float left = 0.0f;
		float right = layout->_width;
		float bottom = layout->_descender;
		float top = layout->_ascender;
		if (_alignByGeometry && _mesh.HasInk())
		{
			left = _mesh._inkMin.GetX();
			right = _mesh._inkMax.GetX();
			bottom = _mesh._inkMin.GetY();
			top = _mesh._inkMax.GetY();
		}

		// Node-local space is centered on the node and Y-up: where the start of the baseline goes
		const Vector2 halfSize = size * 0.5f;
		Vector2       baselineOrigin;
		switch (_horizontalAlignment)
		{
			case HorizontalAlignment::Left: baselineOrigin.SetX(-halfSize.GetX() - left); break;
			case HorizontalAlignment::Center: baselineOrigin.SetX(-(left + right) * 0.5f); break;
			case HorizontalAlignment::Right: baselineOrigin.SetX(halfSize.GetX() - right); break;
		}
		switch (_verticalAlignment)
		{
			case VerticalAlignment::Top: baselineOrigin.SetY(halfSize.GetY() - top); break;
			case VerticalAlignment::Middle: baselineOrigin.SetY(-(bottom + top) * 0.5f); break;
			case VerticalAlignment::Capline: baselineOrigin.SetY(_alignByGeometry ? -(bottom + top) * 0.5f : -layout->_capHeight * 0.5f); break;
			case VerticalAlignment::Bottom: baselineOrigin.SetY(-halfSize.GetY() - bottom); break;
		}
		// Bitmap: the glyphs are on whole pixels from the origin, the origin is too
		baselineOrigin = Vector2(std::round(baselineOrigin.GetX()), std::round(baselineOrigin.GetY()));

		const Material* material = BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);
		const Matrix4&  canvasMatrix = ComputeCanvasMatrix();
		const Matrix4   meshMatrix = canvasMatrix * Matrix4::Translation(baselineOrigin);

		// Clip: the rect of the node. Shrink: what does not fit even at MinFontSize, in the height (the width has an ellipsis)
		const bool clip = _overflow == Overflow::Clip || (_overflow == Overflow::Shrink && requiredSize.GetY() > size.GetY());
		if (clip)
		{
			drawContext.PushClipRect(canvasMatrix, size);
		}

		Sampler::CreateInfo nearestSamplerCreateInfo;
		nearestSamplerCreateInfo._filterMode = FilterMode::Nearest;
		const Sampler* nearestSampler = RhiDevice::GetInstance()->GetSampler(nearestSamplerCreateInfo);

		// One draw per atlas page the glyphs are in, each font drawn its own way
		for (const TextMesh::Batch& batch : _mesh._batches)
		{
			MaterialInstance* materialInstance = MaterialInstance::Create(material);
			materialInstance->SetTexture("image", batch._glyphCache->GetPage(batch._pageIndex));
			if (batch._mode == GlyphCache::Mode::Pixel)
			{
				materialInstance->SetSampler("imageSampler", nearestSampler); // its texels are its pixels
			}
			materialInstance->SetVec4("ubo.color", Vector4(_color.r, _color.g, _color.b, _color.a));

			drawContext.PushMesh(batch._positions.Data(), batch._uvs.Data(), batch._positions.Size(), batch._indices.Data(), batch._indices.Size(), meshMatrix,
			                     materialInstance);
		}

		if (clip)
		{
			drawContext.PopClipRect();
		}
	}
}
