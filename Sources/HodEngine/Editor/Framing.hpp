#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline editor
{
	/// @brief The editor's rule for framing a rect (a node, a device frame, a prefab...) in a 2D view: centered, and
	/// fitted so the rect fills FillRatio of the view on its tightest axis, leaving a margin around it. Whatever shows
	/// a framed rect follows it, live views (see FramingCamera) as well as one-shot shots (thumbnails), so they all
	/// agree. The rect is in the space the view looks at (canvas space, world space...), how it is obtained is up to
	/// the caller.
	class HOD_EDITOR_API Framing
	{
	public:
		/// @brief Share of the view a framed rect fills
		static constexpr float FillRatio = 0.8f;

	public:
		/// @brief Scale (view pixels per unit of the rect's space) that frames a rect of size rectSize in a view of viewSize pixels
		/// @return 0 for a degenerate rect or view
		static float ComputeScale(const Vector2& rectSize, const Vector2& viewSize);

		/// @brief Extent of the space a shot framing a rect of size rectSize covers: the rect plus its margin, same aspect ratio as the rect
		static Vector2 ComputeShotSize(const Vector2& rectSize);
	};

	/// @brief Camera of a 2D editor view, navigated by panning and zooming, and framing rects (see Framing) either
	/// at once or animated there.
	///
	/// The zoom is relative to a unit scale chosen by the view (e.g. a scale fitting its device frame): the view
	/// draws at unitScale * GetZoom() pixels per unit, which keeps the zoom meaningful when that unit scale changes.
	///
	/// An animated framing moves the camera and the zoom along one shared progress, so they arrive together: the zoom
	/// is interpolated geometrically (each step scales the view by the same ratio, as the eye perceives it), and the
	/// position follows the screen-space progress of that zoom, i.e. the motion is a zoom about the one point that stays
	/// still on screen between both framings, rather than a pan racing a zoom.
	class HOD_EDITOR_API FramingCamera
	{
	public:
		static constexpr float MinZoom = 0.05f;
		static constexpr float MaxZoom = 50.0f;

	public:
		const Vector2& GetPosition() const;
		float          GetZoom() const;

		/// @brief Zoom framing a rect of size rectSize in a view of viewSize pixels drawn at unitScale * zoom pixels per unit
		/// @return the current zoom for a degenerate rect or view
		float ComputeFramingZoom(const Vector2& rectSize, const Vector2& viewSize, float unitScale) const;

		/// @brief Frames the rect centered on rectCenter, of size rectSize (see ComputeFramingZoom). A degenerate rect is only centered.
		void Frame(const Vector2& rectCenter, const Vector2& rectSize, const Vector2& viewSize, float unitScale, bool animated);

		/// @brief Goes to position and zoom, at once or animated. Snapping does not count as a move by the user (see HasBeenMoved).
		void Snap(const Vector2& position, float zoom);
		void AnimateTo(const Vector2& position, float zoom);

		/// @brief Moves the camera at once, dropping any framing in progress
		void Pan(const Vector2& delta);

		/// @brief Zooms by wheelDelta notches, eased (Update). Takes over from a framing in progress, from where it currently is.
		void ZoomBy(float wheelDelta);

		/// @brief Advances the framing animation and the zoom easing, once per frame
		void Update(float deltaTime);

		bool IsAnimating() const;

		/// @brief True once anything but Snap moved the camera: until then, a view may keep re-snapping its opening framing
		/// (e.g. while its window gets its final size).
		bool HasBeenMoved() const;

	private:
		Vector2 _position = Vector2::Zero;
		float   _zoom = 1.0f;
		float   _targetZoom = 1.0f; // the wheel's, eased towards outside of a framing animation

		bool    _animating = false;
		float   _progress = 0.0f;
		Vector2 _startPosition = Vector2::Zero;
		float   _startZoom = 1.0f;
		Vector2 _endPosition = Vector2::Zero;
		float   _endZoom = 1.0f;

		bool _moved = false;
	};
}
