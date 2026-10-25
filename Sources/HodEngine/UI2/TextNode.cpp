#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/TextNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/Text/GlyphCache.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextShaper.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(TextNode::Alignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::TopLeft, "TopLeft");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::TopCenter, "TopCenter");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::TopRight, "TopRight");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::MiddleLeft, "MiddleLeft");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::MiddleCenter, "MiddleCenter");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::MiddleRight, "MiddleRight");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::BottomLeft, "BottomLeft");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::BottomCenter, "BottomCenter");
		reflectionDescriptor.AddEnumValue(TextNode::Alignment::BottomRight, "BottomRight");
	}

	DESCRIBE_REFLECTED_CLASS(TextNode, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_FORMAT_TEXT);

		AddPropertyT(reflectionDescriptor, &TextNode::_font, "Font", &TextNode::SetFont);
		AddPropertyT(reflectionDescriptor, &TextNode::_value, "Value", &TextNode::SetValue);
		AddPropertyT(reflectionDescriptor, &TextNode::_fontSize, "FontSize", &TextNode::SetFontSize);
		AddPropertyT(reflectionDescriptor, &TextNode::_alignment, "Alignment", &TextNode::SetAlignment);
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
	TextNode::Alignment TextNode::GetAlignment() const
	{
		return _alignment;
	}

	/// @brief
	/// @param alignment
	void TextNode::SetAlignment(Alignment alignment)
	{
		if (_alignment != alignment)
		{
			_alignment = alignment;
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

	/// @brief The font face is followed by its id: a font reloaded is shaped again
	/// @return
	const TextLayout* TextNode::UpdateLayout() const
	{
		std::shared_ptr<FontResource> fontResource = _font.Lock();
		if (fontResource == nullptr || fontResource->GetFontFace().IsLoaded() == false || _fontSize <= 0.0f)
		{
			return nullptr;
		}

		const FontFace& fontFace = fontResource->GetFontFace();
		if (_layoutDirty || _layoutFontFaceId != fontFace.GetId())
		{
			const FontFace* faces[] = {&fontFace};
			if (Renderer::GetInstance()->GetTextShaper()->Shape(_value, faces, _fontSize, _layout) == false)
			{
				_layout.Clear();
			}
			_layoutFontFaceId = fontFace.GetId();
			_layoutDirty = false;
			_meshDirty = true;
		}
		return &_layout;
	}

	/// @brief
	/// @return
	Vector2 TextNode::GetLayoutSize() const
	{
		return Vector2(_layout._width, _layout._ascender - _layout._descender);
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

		return GetLayoutSize();
	}

	/// @brief
	/// @param drawContext
	void TextNode::DrawSelf(DrawContext& drawContext)
	{
		const TextLayout* layout = UpdateLayout();
		if (layout == nullptr || layout->_glyphs.Empty())
		{
			return;
		}

		const Vector2& size = GetSize();
		const Vector2  requiredSize = GetLayoutSize();

		// Node-local space is centered on the node and Y-up: the line's top-left corner, then the start of its baseline
		Vector2 lineTopLeftCorner;
		switch (_alignment)
		{
			case Alignment::TopLeft: lineTopLeftCorner = Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f); break;
			case Alignment::TopCenter: lineTopLeftCorner = Vector2(-requiredSize.GetX() * 0.5f, size.GetY() * 0.5f); break;
			case Alignment::TopRight: lineTopLeftCorner = Vector2(size.GetX() * 0.5f - requiredSize.GetX(), size.GetY() * 0.5f); break;

			case Alignment::MiddleLeft: lineTopLeftCorner = Vector2(-size.GetX() * 0.5f, requiredSize.GetY() * 0.5f); break;
			case Alignment::MiddleCenter: lineTopLeftCorner = Vector2(-requiredSize.GetX() * 0.5f, requiredSize.GetY() * 0.5f); break;
			case Alignment::MiddleRight: lineTopLeftCorner = Vector2(size.GetX() * 0.5f - requiredSize.GetX(), requiredSize.GetY() * 0.5f); break;

			case Alignment::BottomLeft: lineTopLeftCorner = Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f + requiredSize.GetY()); break;
			case Alignment::BottomCenter: lineTopLeftCorner = Vector2(-requiredSize.GetX() * 0.5f, -size.GetY() * 0.5f + requiredSize.GetY()); break;
			case Alignment::BottomRight: lineTopLeftCorner = Vector2(size.GetX() * 0.5f - requiredSize.GetX(), -size.GetY() * 0.5f + requiredSize.GetY()); break;
		}
		const Vector2 baselineOrigin = lineTopLeftCorner - Vector2(0.0f, layout->_ascender);

		GlyphCache* glyphCache = Renderer::GetInstance()->GetGlyphCache();
		if (_meshDirty || (_meshOrigin == baselineOrigin) == false)
		{
			std::shared_ptr<FontResource> fontResource = _font.Lock();
			const FontFace*               faces[] = {&fontResource->GetFontFace()};
			_mesh.Build(*layout, faces, _fontSize, baselineOrigin, *glyphCache, GlyphCache::Mode::Bitmap);
			_meshOrigin = baselineOrigin;
			_meshDirty = false;
		}

		const Material* material = BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);
		const Matrix4   canvasMatrix = ComputeCanvasMatrix();

		// One draw per atlas page the glyphs are in
		for (const TextMesh::Batch& batch : _mesh._batches)
		{
			MaterialInstance* materialInstance = MaterialInstance::Create(material);
			materialInstance->SetTexture("image", glyphCache->GetPage(batch._pageIndex));
			materialInstance->SetVec4("ubo.color", Vector4(_color.r, _color.g, _color.b, _color.a));

			drawContext.PushMesh(batch._positions.Data(), batch._uvs.Data(), batch._positions.Size(), batch._indices.Data(), batch._indices.Size(), canvasMatrix,
			                     materialInstance);
		}
	}
}
