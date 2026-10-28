#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/CornerRadii.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline ui2
{
	/// @brief Draws the node's rect procedurally, without texture: filled with a color, its corners rounded,
	/// with a border inside its outline, a thickness under it and a drop shadow. Sharp at any size, every
	/// parameter can be changed (or animated) at runtime. Richer looks are made by stacking several of them
	/// (e.g. a button's face and highlight). It has no natural size (see MeasureContent): its size comes from
	/// its LayoutParams. The thickness and the shadow are drawn out of the node's rect, as overflow: the layout
	/// doesn't make room for them.
	class HOD_UI2_API ShapeNode : public Node
	{
		REFLECTED_CLASS(ShapeNode, Node)

	public:

		/// @brief Which way the shadow's blur goes: away from the shape on the axes it names, both ways on the others
		enum class ShadowDirection : uint8_t
		{
			Around,
			Down,
			Up,
			Left,
			Right,
			DownLeft,
			DownRight,
			UpLeft,
			UpRight,
		};
		REFLECTED_ENUM(HOD_UI2_API, ShadowDirection);

	public:

		const Color&		GetFillColor() const;
		void				SetFillColor(const Color& fillColor);

		const CornerRadii&	GetCornerRadii() const;
		void				SetCornerRadii(const CornerRadii& cornerRadii);

		float				GetBorderWidth() const;
		void				SetBorderWidth(float borderWidth);

		const Color&		GetBorderColor() const;
		void				SetBorderColor(const Color& borderColor);

		float				GetDepth() const;
		void				SetDepth(float depth);

		const Color&		GetDepthColor() const;
		void				SetDepthColor(const Color& depthColor);

		const Color&		GetShadowColor() const;
		void				SetShadowColor(const Color& shadowColor);

		const Vector2&		GetShadowOffset() const;
		void				SetShadowOffset(const Vector2& shadowOffset);

		const Vector2&		GetShadowBlur() const;
		void				SetShadowBlur(const Vector2& shadowBlur);

		float				GetShadowSpread() const;
		void				SetShadowSpread(float shadowSpread);

		ShadowDirection		GetShadowDirection() const;
		void				SetShadowDirection(ShadowDirection shadowDirection);

	protected:

		void				DrawSelf(DrawContext& drawContext) override;

	private:

		Color				_fillColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		CornerRadii			_cornerRadii;
		float				_borderWidth = 0.0f; // inside the outline, the fill is what is left
		Color				_borderColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
		float				_depth = 0.0f; // how far down the shape is swept, under itself: its thickness. Pressed, a button can trade it for a move down
		Color				_depthColor = Color(0.75f, 0.75f, 0.75f, 1.0f);
		Color				_shadowColor = Color(0.0f, 0.0f, 0.0f, 0.0f); // no shadow while transparent
		Vector2				_shadowOffset = Vector2(0.0f, -4.0f);
		Vector2				_shadowBlur = Vector2(0.0f, 8.0f); // on each axis, 2 sigma of the gaussian (a CSS box-shadow blur radius). 0 on x: a drop shadow that stays under the shape
		float				_shadowSpread = 0.0f; // grows (or shrinks, negative) the shadow on each side before its blur, as a CSS box-shadow spread
		ShadowDirection		_shadowDirection = ShadowDirection::Down;
	};
}
