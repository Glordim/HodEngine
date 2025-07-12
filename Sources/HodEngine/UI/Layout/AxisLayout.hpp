#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Layout/Layout.hpp"

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API AxisLayout : public Layout
	{
		REFLECTED_CLASS(AxisLayout, Layout)

	public:

		bool		GetControlWidth() const;
		void		SetControlWidth(bool controlWidth);
		bool		GetControlHeight() const;
		void		SetControlHeight(bool controlHeight);

		bool		GetForceExpandWidth() const;
		void		SetForceExpandWidth(bool forceExpandWidth);
		bool		GetForceExpandHeight() const;
		void		SetForceExpandHeight(bool forceExpandHeight);

		bool		GetConsiderScaleWidth() const;
		void		SetConsiderScaleWidth(bool considerScaleWidth);
		bool		GetConsiderScaleHeight() const;
		void		SetConsiderScaleHeight(bool considerScaleHeight);

		float		GetSpacing() const;
		void		SetSpacing(float spacing);

		bool		GetReverseArrangement() const;
		void		SetReverseArrangement(bool reverseArrangement);

	protected:

		void		CalculateSizesForAxis(Axis currentAxis, Axis mainAxis, float& min, float& preferred, float& flexible);
		void		PlaceAndResizeChildren(Axis currentAxis, Axis mainAxis, float min, float preferred, float flexible);
		void		GetChildSizes(Node* childNode, Axis axis, bool controlSize, bool forceExpand, float& min, float& preferred, float& flexible);

	private:

		bool		_controlWidth = false;
		bool		_controlHeight = false;
		bool		_forceExpandWidth = false;
		bool		_forceExpandHeight = false;
		bool		_considerScaleWidth = false;
		bool		_considerScaleHeight = false;
		float		_spacing = 0.0f;
		bool		_reverseArrangement = false;
	};
}
