#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/RectOffsets.hpp>
#include <HodEngine/Renderer/Resource/TextureResource.hpp>

namespace hod::inline ui2
{
	/// @brief Draws a texture stretched over the node's rect, multiplied by a color. Without a texture it
	/// draws the rect filled with the color alone. Its natural size (see MeasureContent) is its texture's.
	///
	/// With Slices (9-slice), the texture is cut in 3x3 by its edges' slices: the corners keep their size,
	/// the sides stretch along their edge only, the center stretches both ways. Which is what a frame or a
	/// button made from a texture needs to be drawn at any size.
	class HOD_UI2_API ImageNode : public Node
	{
		REFLECTED_CLASS(ImageNode, Node)

	public:

												ImageNode();

		const WeakResource<TextureResource>&	GetTexture() const;
		void									SetTexture(const WeakResource<TextureResource>& texture);

		const Color&							GetColor() const;
		void									SetColor(const Color& color);

		const RectOffsets&						GetSlices() const;
		void									SetSlices(const RectOffsets& slices);

		float									GetSliceScale() const;
		void									SetSliceScale(float sliceScale);

		Vector2									MeasureContent(const Vector2& availableSize) const override;

	protected:

		void									DrawSelf(DrawContext& drawContext) override;

	private:

		WeakResource<TextureResource>			_texture;
		Color									_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		RectOffsets								_slices; // in pixels of the texture, from each of its edges. All 0: not sliced
		float									_sliceScale = 1.0f; // node units per pixel of the texture, for the corners and sides
	};
}
