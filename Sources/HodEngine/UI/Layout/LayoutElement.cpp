#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout/LayoutElement.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(LayoutElement, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &LayoutElement::_ignoreLayout, "_ignoreLayout", &LayoutElement::SetIgnoreLayout);
		AddPropertyT(reflectionDescriptor, &LayoutElement::_minSize, "_minSize", &LayoutElement::SetMinSize);
		AddPropertyT(reflectionDescriptor, &LayoutElement::_preferredSize, "_preferredSize", &LayoutElement::SetPreferredSize);
		AddPropertyT(reflectionDescriptor, &LayoutElement::_flexibleSize, "_flexibleSize", &LayoutElement::SetFlexibleSize);
	}

	/// @brief 
	/// @return 
	bool LayoutElement::GetIgnoreLayout() const
	{
		return _ignoreLayout;
	}

	/// @brief 
	/// @param ignoreLayout 
	void LayoutElement::SetIgnoreLayout(bool ignoreLayout)
	{
		if (_ignoreLayout != ignoreLayout)
		{
			_ignoreLayout = ignoreLayout;
			_propertyChangedEvent.Emit(ChangedProperty::IgnoreLayout);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& LayoutElement::GetMinSize() const
	{
		return _minSize;
	}

	/// @brief 
	/// @param minSize 
	void LayoutElement::SetMinSize(const Vector2& minSize)
	{
		if (_minSize != minSize)
		{
			_minSize = minSize;
			if (_minSize.GetX() < 0)
			{
				_minSize.SetX(0.0f);
			}
			if (_minSize.GetY() < 0)
			{
				_minSize.SetY(0.0f);
			}

			_propertyChangedEvent.Emit(ChangedProperty::MinSize);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& LayoutElement::GetPreferredSize() const
	{
		return _preferredSize;
	}

	/// @brief 
	/// @param preferredSize 
	void LayoutElement::SetPreferredSize(const Vector2& preferredSize)
	{
		if (_preferredSize != preferredSize)
		{
			_preferredSize = preferredSize;
			if (_preferredSize.GetX() < 0)
			{
				_preferredSize.SetX(0.0f);
			}
			if (_preferredSize.GetY() < 0)
			{
				_preferredSize.SetY(0.0f);
			}

			_propertyChangedEvent.Emit(ChangedProperty::PreferredSize);
		}
	}

	/// @brief 
	/// @return 
	const Vector2& LayoutElement::GetFlexibleSize() const
	{
		return _flexibleSize;
	}

	/// @brief 
	/// @param flexibleSize 
	void LayoutElement::SetFlexibleSize(const Vector2& flexibleSize)
	{
		if (_flexibleSize != flexibleSize)
		{
			_flexibleSize = flexibleSize;
			if (_flexibleSize.GetX() < 0)
			{
				_flexibleSize.SetX(0.0f);
			}
			if (_flexibleSize.GetY() < 0)
			{
				_flexibleSize.SetY(0.0f);
			}

			_propertyChangedEvent.Emit(ChangedProperty::FlexibleSize);
		}
	}

	/// @brief 
	/// @return 
	LayoutElement::PropertyChangedEvent& LayoutElement::GetPropertyChangedEvent()
	{
		return _propertyChangedEvent;
	}
}
