#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Fitter/ContentSizeFitter.hpp"

#include "HodEngine/UI/Layout/GridLayout.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(ContentSizeFitter, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &ContentSizeFitter::_horizontalFitMode, "_horizontalFitMode", &ContentSizeFitter::SetHorizontalFitMode);
		AddPropertyT(reflectionDescriptor, &ContentSizeFitter::_verticalFitMode, "_verticalFitMode", &ContentSizeFitter::SetVerticalFitMode);
	}

	/// @brief 
	/// @param pComponent 
	ContentSizeFitter::ContentSizeFitter()
		: Fitter()
		, _onDrivenTextChangedSlot(std::bind(&ContentSizeFitter::OnDrivenTextChanged, this))
		, _onDrivenLayoutChangedSlot(std::bind(&ContentSizeFitter::OnDrivenLayoutChanged, this, std::placeholders::_1))
	{
	}

	/// @brief 
	/// @param element 
	void ContentSizeFitter::OnEnable()
	{
		Text* text = GetOwner()->GetComponent<Text>();
		if (text != nullptr)
		{
			text->GetPropertyChangedEvent() += _onDrivenTextChangedSlot;
		}
		else
		{
			Layout* layout = GetOwner()->GetComponent<Layout>();
			if (layout != nullptr)
			{
				layout->GetSizeChangedEvent() += _onDrivenLayoutChangedSlot;
			}
		}

		SetDirty();
	}

	/// @brief 
	void ContentSizeFitter::OnDisable()
	{
		//_onDrivenTextChangeSlot.DisconnectAll();
		//_onDrivenLayoutChangeSlot.DisconnectAll();
	}

	/// @brief 
	/// @return 
	ContentSizeFitter::FitMode ContentSizeFitter::GetHorizontalFitMode() const
	{
		return _horizontalFitMode;
	}

	/// @brief 
	/// @param fitMode 
	void ContentSizeFitter::SetHorizontalFitMode(FitMode fitMode)
	{
		if (_horizontalFitMode != fitMode)
		{
			_horizontalFitMode = fitMode;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	ContentSizeFitter::FitMode ContentSizeFitter::GetVerticalFitMode() const
	{
		return _verticalFitMode;
	}

	/// @brief 
	/// @param fitMode 
	void ContentSizeFitter::SetVerticalFitMode(FitMode fitMode)
	{
		if (_verticalFitMode != fitMode)
		{
			_verticalFitMode = fitMode;
			SetDirty();
		}
	}

	/// @brief 
	bool ContentSizeFitter::Rebuild()
	{
		Node* node = GetOwner()->GetComponent<Node>();
		if (node == nullptr)
		{
			return true;
		}

		if (_horizontalFitMode != FitMode::Unconstrained || _verticalFitMode != FitMode::Unconstrained)
		{
			Text* text = GetOwner()->GetComponent<Text>();
			if (text != nullptr)
			{
				Vector2 size = node->GetDeltaSize();
				Vector2 contentSize = text->GetContentSize();

				if (_horizontalFitMode != FitMode::Unconstrained)
				{
					size.SetX(contentSize.GetX());
				}
				if (_verticalFitMode != FitMode::Unconstrained)
				{
					size.SetY(contentSize.GetY());
				}

				node->SetDeltaSize(size);
			}
			else
			{
				Layout* layout = GetOwner()->GetComponent<Layout>();
				if (layout != nullptr)
				{
					if (layout->GetReflectionDescriptorV().IsCompatible(GridLayout::GetReflectionDescriptor()))
					{
						// TODO BEURK
						GridLayout* gridLayout = static_cast<GridLayout*>(layout);
						if (gridLayout->GetConstraint() == GridLayout::Constraint::Flexible)
						{
							int32_t constraintCount = gridLayout->GetConstraintCount();
							gridLayout->SetConstraint(GridLayout::Constraint::FixedColumnCount);
							gridLayout->SetConstraintCount(static_cast<int32_t>(std::sqrt(static_cast<float>(GetOwner()->GetChildren().size())) + 0.5f));
							gridLayout->Rebuild();
							gridLayout->SetConstraint(GridLayout::Constraint::Flexible);
							gridLayout->SetConstraintCount(constraintCount);
						}
					}

					Vector2 size = node->GetDeltaSize();

					layout->Rebuild();

					if (_horizontalFitMode == FitMode::MinSize)
					{
						size.SetX(layout->GetTotalMinSize().GetX());
					}
					else if (_horizontalFitMode == FitMode::PreferredSize)
					{
						size.SetX(layout->GetTotalPreferredSize().GetX());
					}

					if (_verticalFitMode == FitMode::MinSize)
					{
						size.SetY(layout->GetTotalMinSize().GetY());
					}
					else if (_verticalFitMode == FitMode::PreferredSize)
					{
						size.SetY(layout->GetTotalPreferredSize().GetY());
					}

					node->SetDeltaSize(size);
				}
			}
		}

		return true;
	}

	/// @brief 
	/// @param drivenText 
	/// @param changedProperty 
	void ContentSizeFitter::OnDrivenTextChanged()
	{
		SetDirty();
	}

	/// @brief 
	/// @param drivenText 
	/// @param changedProperty 
	void ContentSizeFitter::OnDrivenLayoutChanged(Layout::ChangedSizeProperty sizeAxis)
	{
		SetDirty();
	}
}
