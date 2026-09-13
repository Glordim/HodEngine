#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"

namespace hod::inline ui2
{
	/// @brief Free/anchored placement within the parent's rect, akin to Unity's RectTransform
	/// anchors. Not tied to the root/Canvas: any node can use this to let a child free-place
	/// itself, as opposed to flow containers (grid, vertical/horizontal box) which will define
	/// their own LayoutParams types.
	class HOD_UI2_API AnchoredLayoutParams : public LayoutParams
	{
		REFLECTED_CLASS(AnchoredLayoutParams, LayoutParams)

	public:

		Vector2						ComputeSize(const Vector2& parentSize, const Vector2& desiredSize) const override;
		Vector2						ComputePosition(const Vector2& parentSize, const Vector2& size) const override;
		bool						DependsOnParentSize() const override;

		const Vector2&				GetAnchorMin() const;
		void						SetAnchorMin(const Vector2& anchorMin);

		const Vector2&				GetAnchorMax() const;
		void						SetAnchorMax(const Vector2& anchorMax);

		const Vector2&				GetPivot() const;
		void						SetPivot(const Vector2& pivot);

		// Additional translation applied on top of the anchor-computed position, in the same
		// spirit as Unity's RectTransform.anchoredPosition. Only meaningful for anchor-based
		// placement, which is why it lives here rather than on Node.
		const Vector2&				GetOffset() const;
		void						SetOffset(const Vector2& offset);

	private:

		Vector2						_anchorMin = Vector2(0.5f, 0.5f);
		Vector2						_anchorMax = Vector2(0.5f, 0.5f);
		Vector2						_pivot = Vector2(0.5f, 0.5f);
		Vector2						_offset = Vector2::Zero;
	};
}
