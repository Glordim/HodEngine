#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Fitter/Fitter.hpp"

#include "HodEngine/UI/Drawables/Text.hpp"
#include "HodEngine/UI/Layout/Layout.hpp"

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API ContentSizeFitter : public Fitter
	{
		REFLECTED_CLASS(ContentSizeFitter, Fitter)

	public:

		enum FitMode
		{
			Unconstrained,
			MinSize,
			PreferredSize,
		};

	public:

		ContentSizeFitter();

		FitMode			GetVerticalFitMode() const;
		void			SetVerticalFitMode(FitMode fitMode);

		FitMode			GetHorizontalFitMode() const;
		void			SetHorizontalFitMode(FitMode fitMode);

		bool			Rebuild() override;

	protected:

		void			OnEnable() override;
		void			OnDisable() override;

	private:

		void			OnDrivenTextChanged();
		void			OnDrivenLayoutChanged(Layout::ChangedSizeProperty sizeAxis);

	private:

		FitMode			_horizontalFitMode = FitMode::Unconstrained;
		FitMode			_verticalFitMode = FitMode::Unconstrained;

		//Text::PropertyChangedEvent::Slot	_onDrivenTextChangedSlot;
		Layout::SizeChangedEvent::Slot		_onDrivenLayoutChangedSlot;
	};
}
