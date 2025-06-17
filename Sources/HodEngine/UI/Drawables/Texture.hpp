#pragma once
#include "HodEngine/UI/Export.hpp"
#include "HodEngine/UI/Drawables/Drawable.hpp"
#include <HodEngine/Core/Resource/WeakResource.hpp>
#include <HodEngine/Renderer/Resource/TextureResource.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API Texture : public Drawable
	{
		REFLECTED_CLASS(Texture, Drawable)

	public:

		void	SetTexture(const WeakResource<renderer::TextureResource>& texture);

		void	PushToRenderQueue(renderer::RenderQueue& renderQueue) override;

	private:

		WeakResource<renderer::TextureResource>	_texture;
		renderer::MaterialInstance* _materialInstance = nullptr;
	};
}
