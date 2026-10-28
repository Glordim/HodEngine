#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/ShapeNode.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>

#include "HodEngine/Core/StaticArray.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(ShapeNode::ShadowDirection, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::Around, "Around");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::Down, "Down");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::Up, "Up");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::Left, "Left");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::Right, "Right");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::DownLeft, "DownLeft");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::DownRight, "DownRight");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::UpLeft, "UpLeft");
		reflectionDescriptor.AddEnumValue(ShapeNode::ShadowDirection::UpRight, "UpRight");
	}

	DESCRIBE_REFLECTED_CLASS(ShapeNode, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_SQUARE_ROUNDED_OUTLINE);

		AddPropertyT(reflectionDescriptor, &ShapeNode::_fillColor, "FillColor", &ShapeNode::SetFillColor);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_cornerRadii, "CornerRadii", &ShapeNode::SetCornerRadii);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_borderWidth, "BorderWidth", &ShapeNode::SetBorderWidth);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_borderColor, "BorderColor", &ShapeNode::SetBorderColor);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_depth, "Depth", &ShapeNode::SetDepth);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_depthColor, "DepthColor", &ShapeNode::SetDepthColor);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_shadowColor, "ShadowColor", &ShapeNode::SetShadowColor);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_shadowOffset, "ShadowOffset", &ShapeNode::SetShadowOffset);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_shadowBlur, "ShadowBlur", &ShapeNode::SetShadowBlur);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_shadowSpread, "ShadowSpread", &ShapeNode::SetShadowSpread);
		AddPropertyT(reflectionDescriptor, &ShapeNode::_shadowDirection, "ShadowDirection", &ShapeNode::SetShadowDirection);
	}

	/// @brief
	/// @return
	const Color& ShapeNode::GetFillColor() const
	{
		return _fillColor;
	}

	/// @brief
	/// @param fillColor
	void ShapeNode::SetFillColor(const Color& fillColor)
	{
		if (_fillColor != fillColor)
		{
			_fillColor = fillColor;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const CornerRadii& ShapeNode::GetCornerRadii() const
	{
		return _cornerRadii;
	}

	/// @brief
	/// @param cornerRadii
	void ShapeNode::SetCornerRadii(const CornerRadii& cornerRadii)
	{
		if (_cornerRadii != cornerRadii)
		{
			_cornerRadii = cornerRadii;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	float ShapeNode::GetBorderWidth() const
	{
		return _borderWidth;
	}

	/// @brief
	/// @param borderWidth negative values are clamped to 0
	void ShapeNode::SetBorderWidth(float borderWidth)
	{
		borderWidth = Max(borderWidth, 0.0f);
		if (_borderWidth != borderWidth)
		{
			_borderWidth = borderWidth;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Color& ShapeNode::GetBorderColor() const
	{
		return _borderColor;
	}

	/// @brief
	/// @param borderColor
	void ShapeNode::SetBorderColor(const Color& borderColor)
	{
		if (_borderColor != borderColor)
		{
			_borderColor = borderColor;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	float ShapeNode::GetDepth() const
	{
		return _depth;
	}

	/// @brief
	/// @param depth negative values are clamped to 0
	void ShapeNode::SetDepth(float depth)
	{
		depth = Max(depth, 0.0f);
		if (_depth != depth)
		{
			_depth = depth;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Color& ShapeNode::GetDepthColor() const
	{
		return _depthColor;
	}

	/// @brief
	/// @param depthColor
	void ShapeNode::SetDepthColor(const Color& depthColor)
	{
		if (_depthColor != depthColor)
		{
			_depthColor = depthColor;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Color& ShapeNode::GetShadowColor() const
	{
		return _shadowColor;
	}

	/// @brief
	/// @param shadowColor
	void ShapeNode::SetShadowColor(const Color& shadowColor)
	{
		if (_shadowColor != shadowColor)
		{
			_shadowColor = shadowColor;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& ShapeNode::GetShadowOffset() const
	{
		return _shadowOffset;
	}

	/// @brief
	/// @param shadowOffset
	void ShapeNode::SetShadowOffset(const Vector2& shadowOffset)
	{
		if (_shadowOffset != shadowOffset)
		{
			_shadowOffset = shadowOffset;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& ShapeNode::GetShadowBlur() const
	{
		return _shadowBlur;
	}

	/// @brief
	/// @param shadowBlur negative values are clamped to 0
	void ShapeNode::SetShadowBlur(const Vector2& shadowBlur)
	{
		const Vector2 clamped(Max(shadowBlur.GetX(), 0.0f), Max(shadowBlur.GetY(), 0.0f));
		if (_shadowBlur != clamped)
		{
			_shadowBlur = clamped;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	float ShapeNode::GetShadowSpread() const
	{
		return _shadowSpread;
	}

	/// @brief
	/// @param shadowSpread
	void ShapeNode::SetShadowSpread(float shadowSpread)
	{
		if (_shadowSpread != shadowSpread)
		{
			_shadowSpread = shadowSpread;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief
	/// @return
	ShapeNode::ShadowDirection ShapeNode::GetShadowDirection() const
	{
		return _shadowDirection;
	}

	/// @brief
	/// @param shadowDirection
	void ShapeNode::SetShadowDirection(ShadowDirection shadowDirection)
	{
		if (_shadowDirection != shadowDirection)
		{
			_shadowDirection = shadowDirection;
			GetPropertyChangedEvent().Emit();
		}
	}

	/// @brief The sign of the blur's direction on each axis, 0 for both ways
	/// @param shadowDirection
	/// @return
	static Vector2 ToVector(ShapeNode::ShadowDirection shadowDirection)
	{
		switch (shadowDirection)
		{
			case ShapeNode::ShadowDirection::Down: return Vector2(0.0f, -1.0f);
			case ShapeNode::ShadowDirection::Up: return Vector2(0.0f, 1.0f);
			case ShapeNode::ShadowDirection::Left: return Vector2(-1.0f, 0.0f);
			case ShapeNode::ShadowDirection::Right: return Vector2(1.0f, 0.0f);
			case ShapeNode::ShadowDirection::DownLeft: return Vector2(-1.0f, -1.0f);
			case ShapeNode::ShadowDirection::DownRight: return Vector2(1.0f, -1.0f);
			case ShapeNode::ShadowDirection::UpLeft: return Vector2(-1.0f, 1.0f);
			case ShapeNode::ShadowDirection::UpRight: return Vector2(1.0f, 1.0f);
			default: return Vector2(0.0f, 0.0f);
		}
	}

	/// @brief One quad the shader evaluates the shape's distance field on (P2fT2f_Shape), grown to hold the shadow
	/// @param drawContext
	void ShapeNode::DrawSelf(DrawContext& drawContext)
	{
		const Vector2& size = GetSize();
		const Vector2  halfSize(0.5f * size.GetX(), 0.5f * size.GetY());

		// The quad holds the rect and its thickness under it, plus the shadow where it goes past them: grown by its spread, moved
		// by its offset, and 3 sigma of its gaussian further (1.5 blur). One more unit keeps room for the antialiasing of the outlines
		float left = halfSize.GetX();
		float right = halfSize.GetX();
		float top = halfSize.GetY();
		float bottom = halfSize.GetY() + _depth;
		if (_shadowColor.a > 0.0f)
		{
			float shadowExtentX = _shadowSpread + 1.5f * _shadowBlur.GetX();
			float shadowExtentY = _shadowSpread + 1.5f * _shadowBlur.GetY();
			left = Max(left, halfSize.GetX() - _shadowOffset.GetX() + shadowExtentX);
			right = Max(right, halfSize.GetX() + _shadowOffset.GetX() + shadowExtentX);
			top = Max(top, halfSize.GetY() + _shadowOffset.GetY() + shadowExtentY);
			bottom = Max(bottom, halfSize.GetY() + _depth - _shadowOffset.GetY() + shadowExtentY);
		}
		left += 1.0f;
		right += 1.0f;
		top += 1.0f;
		bottom += 1.0f;

		// The uvs are the positions themselves: the shader works relative to the rect's center
		const StaticArray<Vector2, 4> vertices = {
			Vector2(-left, top),
			Vector2(right, top),
			Vector2(right, -bottom),
			Vector2(-left, -bottom),
		};
		static const StaticArray<uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};

		MaterialInstance* materialInstance =
			MaterialInstance::Create(BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Shape));
		materialInstance->SetVec4("ubo.fillColor", Vector4(_fillColor.r, _fillColor.g, _fillColor.b, _fillColor.a));
		materialInstance->SetVec4("ubo.borderColor", Vector4(_borderColor.r, _borderColor.g, _borderColor.b, _borderColor.a));
		materialInstance->SetVec4("ubo.shadowColor", Vector4(_shadowColor.r, _shadowColor.g, _shadowColor.b, _shadowColor.a));
		materialInstance->SetVec4("ubo.depthColor", Vector4(_depthColor.r, _depthColor.g, _depthColor.b, _depthColor.a));
		materialInstance->SetVec4("ubo.cornerRadii", Vector4(_cornerRadii._topLeft, _cornerRadii._topRight, _cornerRadii._bottomRight, _cornerRadii._bottomLeft));
		materialInstance->SetVec2("ubo.halfSize", halfSize);
		materialInstance->SetVec2("ubo.shadowOffset", _shadowOffset);
		materialInstance->SetFloat("ubo.borderWidth", _borderWidth);
		materialInstance->SetVec2("ubo.shadowBlur", _shadowBlur);
		materialInstance->SetFloat("ubo.shadowSpread", _shadowSpread);
		materialInstance->SetVec2("ubo.shadowDirection", ToVector(_shadowDirection));
		materialInstance->SetFloat("ubo.depth", _depth);

		drawContext.PushMesh(vertices.Data(), vertices.Data(), (uint32_t)vertices.Size(), indices.Data(), (uint32_t)indices.Size(), ComputeCanvasMatrix(), materialInstance);
	}
}
