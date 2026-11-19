#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/LayoutParams.hpp"
#include "HodEngine/UI/Node.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitEnabledIf.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitVectorComponentEnabledIf.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitVectorComponentLabels.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_ENUM(LayoutParams::SizeMode)
	{
		HOD_ENUM_VALUE(Fixed);
		HOD_ENUM_VALUE(Content);
	}

	DESCRIBE_REFLECTED_CLASS(LayoutParams)
	{
		HOD_PROPERTY(_widthMode, &LayoutParams::SetWidthMode)
		    ->AddTrait<ReflectionTraitEnabledIf>(&LayoutParams::IsWidthModeUsed, "Unused: the width is stretched, the anchors and the size delta give it", "_anchorMin", "_anchorMax");
		HOD_PROPERTY(_heightMode, &LayoutParams::SetHeightMode)
		    ->AddTrait<ReflectionTraitEnabledIf>(&LayoutParams::IsHeightModeUsed, "Unused: the height is stretched, the anchors and the size delta give it", "_anchorMin", "_anchorMax");
		ReflectionProperty* sizeProperty = HOD_PROPERTY(_size, &LayoutParams::SetSize);
		sizeProperty->AddTrait<ReflectionTraitVectorComponentLabels>("Width", "Height");
		sizeProperty->AddTrait<ReflectionTraitVectorComponentEnabledIf>(&LayoutParams::IsSizeUsedX, "Unused: the width is sized from the content (Content mode)", "_widthMode",
		                                                                &LayoutParams::IsSizeUsedY, "Unused: the height is sized from the content (Content mode)", "_heightMode");
		HOD_PROPERTY(_minSize, &LayoutParams::SetMinSize)->AddTrait<ReflectionTraitVectorComponentLabels>("Width", "Height");
		HOD_PROPERTY(_maxSize, &LayoutParams::SetMaxSize)->AddTrait<ReflectionTraitVectorComponentLabels>("Width", "Height");
	}

	LayoutParams::SizeMode LayoutParams::GetWidthMode() const
	{
		return _widthMode;
	}

	void LayoutParams::SetWidthMode(SizeMode widthMode)
	{
		if (_widthMode != widthMode)
		{
			_widthMode = widthMode;
			MarkOwnerMeasureAsDirty();
		}
	}

	LayoutParams::SizeMode LayoutParams::GetHeightMode() const
	{
		return _heightMode;
	}

	void LayoutParams::SetHeightMode(SizeMode heightMode)
	{
		if (_heightMode != heightMode)
		{
			_heightMode = heightMode;
			MarkOwnerMeasureAsDirty();
		}
	}

	bool LayoutParams::IsWidthModeUsed() const
	{
		return true;
	}

	bool LayoutParams::IsHeightModeUsed() const
	{
		return true;
	}

	bool LayoutParams::IsSizeUsedX() const
	{
		return _widthMode == SizeMode::Fixed;
	}

	bool LayoutParams::IsSizeUsedY() const
	{
		return _heightMode == SizeMode::Fixed;
	}

	const Vector2& LayoutParams::GetSize() const
	{
		return _size;
	}

	void LayoutParams::SetSize(const Vector2& size)
	{
		if (_size != size)
		{
			_size = size;
			MarkOwnerMeasureAsDirty();
		}
	}

	const Vector2& LayoutParams::GetMinSize() const
	{
		return _minSize;
	}

	void LayoutParams::SetMinSize(const Vector2& minSize)
	{
		if (_minSize != minSize)
		{
			_minSize = minSize;
			MarkOwnerMeasureAsDirty();
		}
	}

	const Vector2& LayoutParams::GetMaxSize() const
	{
		return _maxSize;
	}

	void LayoutParams::SetMaxSize(const Vector2& maxSize)
	{
		if (_maxSize != maxSize)
		{
			_maxSize = maxSize;
			MarkOwnerMeasureAsDirty();
		}
	}

	void LayoutParams::CarryOverFrom(const LayoutParams& previous)
	{
		(void)previous;
	}

	bool LayoutParams::DependsOnParentSize() const
	{
		return DependsOnParentWidth() || DependsOnParentHeight();
	}

	bool LayoutParams::DependsOnParentWidth() const
	{
		return false;
	}

	bool LayoutParams::DependsOnParentHeight() const
	{
		return false;
	}

	/// @brief
	/// @param contentSize
	/// @return
	Vector2 LayoutParams::ResolveSize(const Vector2& contentSize) const
	{
		float width = _widthMode == SizeMode::Fixed ? _size.GetX() : contentSize.GetX();
		float height = _heightMode == SizeMode::Fixed ? _size.GetY() : contentSize.GetY();
		return Vector2(Clamp(width, _minSize.GetX(), _maxSize.GetX()), Clamp(height, _minSize.GetY(), _maxSize.GetY()));
	}

	/// @brief
	/// @param availableSize
	/// @param contentSize
	/// @return
	Vector2 LayoutParams::ComputeSize(const Vector2& availableSize, const Vector2& contentSize) const
	{
		(void)availableSize;
		return ResolveSize(contentSize);
	}

	bool LayoutParams::TryComputePosition(const Rect& parentContentRect, const Vector2& size, Vector2& outPosition) const
	{
		(void)parentContentRect;
		(void)size;
		(void)outPosition;
		return false;
	}

	void LayoutParams::MarkOwnerMeasureAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkMeasureAsDirty();
		}
	}

	Node* LayoutParams::GetNode() const
	{
		return _node;
	}

	void LayoutParams::MarkOwnerArrangeAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkArrangeAsDirty();
		}
	}
}
