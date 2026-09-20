#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/TextNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/Font/Font.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Renderer.hpp>

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
		AddPropertyT(reflectionDescriptor, &TextNode::_font, "Font", &TextNode::SetFont);
		AddPropertyT(reflectionDescriptor, &TextNode::_value, "Value", &TextNode::SetValue);
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
			MarkMeasureAsDirty(); // the text bounds depend on the text
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

	/// @brief The text's bounds once a font is set, the desired size before that.
	/// @param availableSize
	/// @return
	Vector2 TextNode::MeasureContent(const Vector2& availableSize) const
	{
		std::shared_ptr<FontResource> fontResource = _font.Lock();
		if (fontResource == nullptr || fontResource->GetFont() == nullptr)
		{
			return Node::MeasureContent(availableSize);
		}

		return fontResource->GetFont()->ComputeRequiredSize(_value);
	}

	/// @brief
	/// @param drawContext
	void TextNode::DrawSelf(DrawContext& drawContext)
	{
		std::shared_ptr<FontResource> fontResource = _font.Lock();
		if (fontResource == nullptr || fontResource->GetFont() == nullptr || _value.Empty())
		{
			return;
		}

		Font*          font = fontResource->GetFont();
		const Vector2& size = GetSize();

		Vector2 requiredSize = font->ComputeRequiredSize(_value);

		// Node-local space is centered on the node and Y-up: the line's top-left corner is the origin
		// the glyph geometry is expressed from.
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

		Vector<Font::GlyphGeometry> glyphGeometries;
		font->BuildTextGeometry(_value, glyphGeometries);

		Vector<Vector2>  positions;
		Vector<Vector2>  uvs;
		Vector<uint16_t> indices;
		positions.Reserve(glyphGeometries.Size() * 4);
		uvs.Reserve(glyphGeometries.Size() * 4);
		indices.Reserve(glyphGeometries.Size() * 6);

		for (const Font::GlyphGeometry& glyphGeometry : glyphGeometries)
		{
			uint16_t vertexCount = (uint16_t)positions.Size();

			Vector2 halfSize = glyphGeometry._posSize * 0.5f;
			Vector2 center = lineTopLeftCorner + glyphGeometry._posCenter;

			positions.PushBack(center + Vector2(-halfSize.GetX(), halfSize.GetY()));
			positions.PushBack(center + Vector2(halfSize.GetX(), halfSize.GetY()));
			positions.PushBack(center + Vector2(-halfSize.GetX(), -halfSize.GetY()));
			positions.PushBack(center + Vector2(halfSize.GetX(), -halfSize.GetY()));

			uvs.PushBack(glyphGeometry._uvPos);
			uvs.PushBack(glyphGeometry._uvPos + Vector2(glyphGeometry._uvSize.GetX(), 0.0f));
			uvs.PushBack(glyphGeometry._uvPos + Vector2(0.0f, glyphGeometry._uvSize.GetY()));
			uvs.PushBack(glyphGeometry._uvPos + Vector2(glyphGeometry._uvSize.GetX(), glyphGeometry._uvSize.GetY()));

			indices.PushBack((uint16_t)(vertexCount + 1));
			indices.PushBack((uint16_t)(vertexCount + 0));
			indices.PushBack((uint16_t)(vertexCount + 2));

			indices.PushBack((uint16_t)(vertexCount + 1));
			indices.PushBack((uint16_t)(vertexCount + 2));
			indices.PushBack((uint16_t)(vertexCount + 3));
		}

		if (positions.Empty())
		{
			return;
		}

		MaterialInstance* materialInstance =
			Renderer::GetInstance()->CreateMaterialInstance(MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color));
		materialInstance->SetTexture("image", font->GetTexture());
		materialInstance->SetVec4("ubo.color", Vector4(_color.r, _color.g, _color.b, _color.a));

		drawContext.PushMesh(positions.Data(), uvs.Data(), positions.Size(), indices.Data(), indices.Size(), ComputeCanvasMatrix(), materialInstance);
	}
}
