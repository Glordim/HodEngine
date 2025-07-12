#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout/GridLayout.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(GridLayout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &GridLayout::_cellSize, "_cellSize", &GridLayout::SetCellSize);
		AddPropertyT(reflectionDescriptor, &GridLayout::_cellSpacing, "_cellSpacing", &GridLayout::SetCellSpacing);
		AddPropertyT(reflectionDescriptor, &GridLayout::_constraint, "_constraint", &GridLayout::SetConstraint);
		AddPropertyT(reflectionDescriptor, &GridLayout::_constraintCount, "_constraintCount", &GridLayout::SetConstraintCount);
		AddPropertyT(reflectionDescriptor, &GridLayout::_startAxis, "_startAxis", &GridLayout::SetStartAxis);
		AddPropertyT(reflectionDescriptor, &GridLayout::_startCorner, "_startCorner", &GridLayout::SetStartCorner);
	}

	/// @brief 
	/// @return 
	const Vector2& GridLayout::GetCellSize() const
	{
		return _cellSize;
	}

	/// @brief 
	/// @param cellSize 
	void GridLayout::SetCellSize(const Vector2& cellSize)
	{
		if (_cellSize != cellSize)
		{
			_cellSize = cellSize;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	const Vector2& GridLayout::GetCellSpacing() const
	{
		return _cellSpacing;
	}

	/// @brief 
	/// @param cellSpacing 
	void GridLayout::SetCellSpacing(const Vector2& cellSpacing)
	{
		if (_cellSpacing != cellSpacing)
		{
			_cellSpacing = cellSpacing;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	GridLayout::Constraint GridLayout::GetConstraint() const
	{
		return _constraint;
	}

	/// @brief 
	/// @param constraint 
	void GridLayout::SetConstraint(Constraint constraint)
	{
		if (_constraint != constraint)
		{
			_constraint = constraint;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	int32_t GridLayout::GetConstraintCount() const
	{
		return _constraintCount;
	}

	/// @brief 
	/// @param constraintCount 
	void GridLayout::SetConstraintCount(int32_t constraintCount)
	{
		if (_constraintCount != constraintCount)
		{
			_constraintCount = constraintCount;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	GridLayout::Axis GridLayout::GetStartAxis() const
	{
		return _startAxis;
	}

	/// @brief 
	/// @param startAxis 
	void GridLayout::SetStartAxis(Axis startAxis)
	{
		if (_startAxis != startAxis)
		{
			_startAxis = startAxis;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	GridLayout::Corner GridLayout::GetStartCorner() const
	{
		return _startCorner;
	}

	/// @brief 
	/// @param startCorner 
	void GridLayout::SetStartCorner(Corner startCorner)
	{
		if (_startCorner != startCorner)
		{
			_startCorner = startCorner;
			SetDirty();
		}
	}

	/// @brief 
	void GridLayout::ComputeChildrenPositionAndSize()
	{
		Node* node = GetOwner()->GetComponent<Node>();
		if (node == nullptr)
		{
			return;
		}

		Vector2 availableSize = node->ComputeSize();
		const Vector<WeakPtr<Node>>& drivenNodes = GetDrivenNodes();
		uint32_t drivenNodeCount = drivenNodes.size();

		uint32_t cellCountX = 1;
		uint32_t cellCountY = 1;

		if (_constraint == Constraint::FixedColumnCount)
		{
			cellCountX = _constraintCount;

			if (drivenNodeCount > cellCountX)
			{
				cellCountY = drivenNodeCount / cellCountX;
				if (drivenNodeCount % cellCountX != 0)
				{
					++cellCountY;
				}
			}
		}
		else if (_constraint == Constraint::FixedRowCount)
		{
			cellCountY = _constraintCount;

			if (drivenNodeCount > cellCountY)
			{
				cellCountX = drivenNodeCount / cellCountY;
				if (drivenNodeCount % cellCountY != 0)
				{
					++cellCountX;
				}
			}
		}
		else
		{
			if (_cellSize.GetX() + _cellSpacing.GetX() >= 0)
			{
				float availableWidth = availableSize.GetX() - (GetPadding().GetLeft() + GetPadding().GetRight()) + _cellSpacing.GetX() + 0.0001f;

				cellCountX = std::max(1, (int)(std::floor(availableWidth / (_cellSize.GetX() + _cellSpacing.GetX()))));
			}

			if (_cellSize.GetY() + _cellSpacing.GetY() >= 0)
			{
				float availableHeight = availableSize.GetY() - (GetPadding().GetTop() + GetPadding().GetBottom()) + _cellSpacing.GetY() + 0.0001f;

				cellCountY =std::max(1, (int)(std::floor(availableHeight / (_cellSize.GetY() + _cellSpacing.GetY()))));
			}
		}

		int32_t cornerX = (int32_t)_startCorner % 2;
		int32_t cornerY = (int32_t)_startCorner / 2;

		int32_t cellsPerMainAxis;
		uint32_t actualCellCountX;
		uint32_t actualCellCountY;

		if (_startAxis == Axis::Horizontal)
		{
			cellsPerMainAxis = cellCountX;
			actualCellCountX = std::clamp<uint32_t>(cellCountX, 1, drivenNodeCount);
			actualCellCountY = std::clamp<uint32_t>(cellCountY, 1, (uint32_t)std::ceil((float)drivenNodeCount / (float)cellsPerMainAxis));
		}
		else
		{
			cellsPerMainAxis = cellCountY;
			actualCellCountY = std::clamp<uint32_t>(cellCountY, 1, drivenNodeCount);
			actualCellCountX = std::clamp<uint32_t>(cellCountX, 1, (uint32_t)std::ceil((float)drivenNodeCount / (float)cellsPerMainAxis));
		}

		float requiredSpaceX = actualCellCountX * _cellSize.GetX() + (actualCellCountX - 1) * _cellSpacing.GetX() + (GetPadding().GetLeft() + GetPadding().GetRight());
		float requiredSpaceY = actualCellCountY * _cellSize.GetY() + (actualCellCountY - 1) * _cellSpacing.GetY() + (GetPadding().GetTop() + GetPadding().GetBottom());

		SetTotalSizeX(requiredSpaceX, requiredSpaceX, requiredSpaceX);
		SetTotalSizeY(requiredSpaceY, requiredSpaceY, requiredSpaceY);

		float startOffsetX = GetStartOffset(Axis::Horizontal, requiredSpaceX, availableSize.GetX());
		float startOffsetY = GetStartOffset(Axis::Vertical, requiredSpaceY, availableSize.GetY());

		for (uint32_t drivenNodeIndex = 0; drivenNodeIndex < drivenNodeCount; ++drivenNodeIndex)
		{
			int32_t posX;
			int32_t posY;

			if (_startAxis == Axis::Horizontal)
			{
				posX = drivenNodeIndex % cellsPerMainAxis;
				posY = drivenNodeIndex / cellsPerMainAxis;
			}
			else
			{
				posX = drivenNodeIndex / cellsPerMainAxis;
				posY = drivenNodeIndex % cellsPerMainAxis;
			}

			if (cornerX == 1)
			{
				posX = actualCellCountX - 1 - posX;
			}
			if (cornerY == 1)
			{
				posY = actualCellCountY - 1 - posY;
			}

			Node* childNode = drivenNodes[drivenNodeIndex].Get();
			if (childNode != nullptr)
			{
				SetChildAlongAxis(childNode, Axis::Horizontal, startOffsetX + (_cellSize.GetX() + _cellSpacing.GetX()) * posX);
				SetChildAlongAxis(childNode, Axis::Vertical, startOffsetY + (_cellSize.GetY() + _cellSpacing.GetY()) * posY);
			}
		}
	}
}
