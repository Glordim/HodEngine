#pragma once
#include "HodEngine/UI/Export.hpp"
#include "HodEngine/UI/Drawables/Drawable.hpp"
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/Renderer/Resource/TextureResource.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

namespace hod::inline ui
{
	/// @brief 
	class HOD_UI_API Texture : public Drawable
	{
		REFLECTED_CLASS(Texture, Drawable)

	public:

		void	OnDestruct() override;

		void	SetTexture(const WeakResource<TextureResource>& texture);

		void	PushRenderCommand(RenderView& renderView, RenderView::RenderQueueType renderQueueType) override;

	private:

		WeakResource<TextureResource>	_texture;
		MaterialInstance* _materialInstance = nullptr;
	};
}
