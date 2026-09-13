#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"

namespace hod::inline ui2
{
	class Canvas;
}

namespace hod::inline game
{
	/// @brief 
	class HOD_GAME_API UIComponent : public Component
	{
		REFLECTED_CLASS(UIComponent, Component)

	public:

										UIComponent();
										UIComponent(const UIComponent&) = delete;
										UIComponent(UIComponent&&) = delete;
										~UIComponent() override;

		UIComponent&					operator=(const UIComponent&) = delete;
		UIComponent&					operator=(UIComponent&&) = delete;

		void	OnConstruct() override;
		void	OnDestruct() override;

	private:

		//WeakResource<UIPrefabResource>	_canvas;
		ui2::Canvas* _canvas;
	};
}
