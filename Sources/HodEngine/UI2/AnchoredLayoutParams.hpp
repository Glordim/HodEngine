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
	///
	/// Each axis is either:
	/// - anchored on a point (AnchorMin == AnchorMax on that axis): the child's size is its own
	///   (LayoutParams' SizeMode/Size), and its Pivot is put on the anchor point, moved by Offset.
	/// - stretched (AnchorMin != AnchorMax): the child spans its anchors, grown by Size on that axis
	///   (a delta, like Unity's sizeDelta) around its Pivot, put on the matching point between the
	///   anchors and moved by Offset. SizeMode is unused on that axis.
	/// No margin: Offset and the size delta already express any inset (a margin only means something
	/// in a flow container, see BoxLayoutParams).
	class HOD_UI2_API AnchoredLayoutParams : public LayoutParams
	{
		REFLECTED_CLASS(AnchoredLayoutParams, LayoutParams)

	public:

		bool						DependsOnParentWidth() const override;
		bool						DependsOnParentHeight() const override;
		Vector2						ComputeSize(const Vector2& availableSize, const Vector2& contentSize) const override;
		bool						TryComputePosition(const Rect& parentContentRect, const Vector2& size, Vector2& outPosition) const override;

		bool						IsStretchedX() const;
		bool						IsStretchedY() const;

		const Vector2&				GetAnchorMin() const;
		void						SetAnchorMin(const Vector2& anchorMin);

		const Vector2&				GetAnchorMax() const;
		void						SetAnchorMax(const Vector2& anchorMax);

		// Point of the child put on its anchor point, or on the same point between its anchors on a stretched axis.
		const Vector2&				GetPivot() const;
		void						SetPivot(const Vector2& pivot);

		// Translation applied on top of the anchor-computed position, in the same spirit as Unity's
		// RectTransform.anchoredPosition.
		const Vector2&				GetOffset() const;
		void						SetOffset(const Vector2& offset);

		/// @brief Sets Offset and Size (a delta on the stretched axes) so that the child ends up on the
		/// given rect, with the current anchors and pivot. A Content
		/// axis anchored on a point whose size changes becomes Fixed: the size is the author's from then on.
		/// @param parentSize size of the parent's content rect
		/// @param center the rect's center, in the parent's local space (see Node::GetPosition)
		/// @param size the rect's size
		void						PlaceRect(const Vector2& parentSize, const Vector2& center, const Vector2& size);

	private:

		Vector2						_anchorMin = Vector2(0.5f, 0.5f);
		Vector2						_anchorMax = Vector2(0.5f, 0.5f);
		Vector2						_pivot = Vector2(0.5f, 0.5f);
		Vector2						_offset = Vector2::Zero;
	};
}
