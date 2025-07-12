#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>
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

		const Vector2&				GetMinSize() const;
		void						SetMinSize(const Vector2& minSize);

		const Vector2&				GetPreferredSize() const;
		void						SetPreferredSize(const Vector2& preferredSize);

		const Vector2&				GetFlexibleSize() const;
		void						SetFlexibleSize(const Vector2& flexibleSize);

		PropertyChangedEvent&		GetPropertyChangedEvent();

	private:

		bool						_ignoreLayout = false;

		Vector2						_minSize;
		Vector2						_preferredSize;
		Vector2						_flexibleSize;

		PropertyChangedEvent		_propertyChangedEvent;
	};
}
