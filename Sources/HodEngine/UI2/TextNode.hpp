#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>

namespace hod::inline ui2
{
	/// @brief Draws a single line of text, aligned within the node's rect. Its natural size (see
	/// MeasureContent) is the text's bounds, so under a Layout it fits its text unless stretched.
	class HOD_UI2_API TextNode : public Node
	{
		REFLECTED_CLASS(TextNode, Node)

	public:

		enum class Alignment : uint8_t
		{
			TopLeft,
			TopCenter,
			TopRight,
			MiddleLeft,
			MiddleCenter,
			MiddleRight,
			BottomLeft,
			BottomCenter,
			BottomRight,
		};
		REFLECTED_ENUM(HOD_UI2_API, Alignment);

	public:

		const WeakResource<FontResource>&	GetFont() const;
		void								SetFont(const WeakResource<FontResource>& font);

		const String&						GetValue() const;
		void								SetValue(const String& value);

		Alignment							GetAlignment() const;
		void								SetAlignment(Alignment alignment);

		const Color&						GetColor() const;
		void								SetColor(const Color& color);

		Vector2								MeasureContent(const Vector2& availableSize) const override;

	protected:

		void								DrawSelf(DrawContext& drawContext) override;

	private:

		WeakResource<FontResource>			_font;
		String								_value;
		Alignment							_alignment = Alignment::MiddleCenter;
		Color								_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	};
}
