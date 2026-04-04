#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Core/Event.hpp>

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API LayoutElement : public game::Component
	{
		REFLECTED_CLASS(LayoutElement, game::Component)

	public:

		enum ChangedProperty
		{
			IgnoreLayout = 0,
			MinSize,
			PreferredSize,
			FlexibleSize,
		};

		using PropertyChangedEvent = Event<ChangedProperty>;

	public:

		bool						GetIgnoreLayout() const;
		void						SetIgnoreLayout(bool ignoreLayout);

		const math::Vector2&				GetMinSize() const;
		void						SetMinSize(const math::Vector2& minSize);

		const math::Vector2&				GetPreferredSize() const;
		void						SetPreferredSize(const math::Vector2& preferredSize);

		const math::Vector2&				GetFlexibleSize() const;
		void						SetFlexibleSize(const math::Vector2& flexibleSize);

		PropertyChangedEvent&		GetPropertyChangedEvent();

	private:

		bool						_ignoreLayout = false;

		math::Vector2						_minSize;
		math::Vector2						_preferredSize;
		math::Vector2						_flexibleSize;

		PropertyChangedEvent		_propertyChangedEvent;
	};
}
