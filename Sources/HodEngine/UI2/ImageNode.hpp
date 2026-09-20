#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Renderer/Resource/TextureResource.hpp>

namespace hod::inline ui2
{
	/// @brief Draws a texture stretched over the node's rect, multiplied by a color. Without a texture it
	/// draws the rect filled with the color alone.
	class HOD_UI2_API ImageNode : public Node
	{
		REFLECTED_CLASS(ImageNode, Node)

	public:

		const WeakResource<TextureResource>&	GetTexture() const;
		void									SetTexture(const WeakResource<TextureResource>& texture);

		const Color&							GetColor() const;
		void									SetColor(const Color& color);

	protected:

		void									DrawSelf(DrawContext& drawContext) override;

	private:

		WeakResource<TextureResource>			_texture;
		Color									_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	};
}
