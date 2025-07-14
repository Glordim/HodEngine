#pragma once
#include "HodEngine/UI/Export.hpp"
#include "HodEngine/UI/Drawables/Drawable.hpp"
#include <HodEngine/Core/Resource/WeakResource.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API Text : public Drawable
	{
		REFLECTED_CLASS(Text, Drawable)

	public:

		enum Alignment
		{
			TopLeft,
			TopCenter,
			TopRight,
			MiddleLeft,
			MiddleCenter,
			MiddleRight,
			BottomLeft,
			BottomCenter,
			BottomRight
		};

		using PropertyChangedEvent = Event<>;

	public:

		void	OnDestruct() override;

		void	SetFont(const WeakResource<renderer::FontResource>& font);
		void	SetValue(const String& value);
		void	SetAlignment(Alignment alignment);

		void	PushRenderCommand(renderer::RenderView& renderView, renderer::RenderView::RenderQueueType renderQueueType) override;

		Vector2	GetContentSize() const;

		PropertyChangedEvent&		GetPropertyChangedEvent();

	private:

		WeakResource<renderer::FontResource>	_font;
		String									_value;
		Alignment								_alignment;

		renderer::MaterialInstance* _materialInstance = nullptr;

		PropertyChangedEvent		_propertyChangedEvent;
	};
}
