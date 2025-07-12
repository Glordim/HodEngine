#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Layout/Layout.hpp"

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API GridLayout : public Layout
	{
		REFLECTED_CLASS(GridLayout, Layout)

	public:

		enum Constraint
		{
			Flexible = 0,
			FixedColumnCount,
			FixedRowCount,
		};

		enum Corner
		{
			TopLeft = 0,
			TopRight,
			BottomLeft,
			BottomRight,
		};

	public:

		const Vector2&	GetCellSize() const;
		void			SetCellSize(const Vector2& cellSize);

		const Vector2&	GetCellSpacing() const;
		void			SetCellSpacing(const Vector2& cellSpacing);

		Constraint		GetConstraint() const;
		void			SetConstraint(Constraint constraint);

		int32_t			GetConstraintCount() const;
		void			SetConstraintCount(int32_t constraintCount);

		Axis			GetStartAxis() const;
		void			SetStartAxis(Axis startAxis);

		Corner			GetStartCorner() const;
		void			SetStartCorner(Corner startCorner);

	protected:

		void			ComputeChildrenPositionAndSize() override;

	private:

		Vector2			_cellSize = Vector2(100.0f, 100.0f);
		Vector2			_cellSpacing;

		Constraint		_constraint = Constraint::Flexible;
		int32_t			_constraintCount = 0;

		Axis			_startAxis = Axis::Horizontal;
		Corner			_startCorner = Corner::TopLeft;
	};
}
