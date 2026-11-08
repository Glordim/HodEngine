#pragma once
#include "HodEngine/UI/Export.hpp"
#include "HodEngine/UI/Interaction.hpp"
#include "HodEngine/UI/NodeRef.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline ui
{
	/// @brief Scrolls a content larger than its viewport, by dragging it (from anywhere in it, a Button pressed included: the
	/// drag takes the press over, see PointerEvent::DragBegin) or with the wheel. The content is a node of its prefab placed by
	/// anchors (AnchoredLayoutParams) in its parent, the viewport, whose rect it scrolls within: the ScrollView's node itself
	/// typically, a MaskNode not to show the content outside. Scrolling moves the content's Offset.
	///
	/// The content can't go further than its edges (Clamped), or can, pulled back when released (Elastic). Released while
	/// moving, it goes on and slows down (Inertia). A content smaller than the viewport sticks to its top / left edge.
	class HOD_UI_API ScrollView : public Interaction
	{
		REFLECTED_CLASS(ScrollView, Interaction)

	public:

		enum class MovementType : uint8_t
		{
			Clamped, // Never past its edges
			Elastic, // Past them while dragged (resisting), pulled back when released
		};
		REFLECTED_ENUM(HOD_UI_API, MovementType);

		using ScrolledEvent = Event<const Vector2&>;

	public:

		bool				GetHorizontal() const;
		void				SetHorizontal(bool horizontal);

		bool				GetVertical() const;
		void				SetVertical(bool vertical);

		MovementType		GetMovementType() const;
		void				SetMovementType(MovementType movementType);

		// Elastic: how long (seconds) a content past its edges takes to come back, about
		float				GetElasticity() const;
		void				SetElasticity(float elasticity);

		bool				GetInertia() const;
		void				SetInertia(bool inertia);

		// Inertia: the part of the speed left after a second (0: stops at once, 1: never slows down)
		float				GetDecelerationRate() const;
		void				SetDecelerationRate(float decelerationRate);

		// Canvas units scrolled per wheel notch
		float				GetScrollSensitivity() const;
		void				SetScrollSensitivity(float scrollSensitivity);

		// Where the viewport is over the content, per axis: 0 at its left / bottom edge, 1 at its right / top one (as laid out
		// last). Set, the content moves there on the next Update.
		Vector2				GetNormalizedPosition();
		void				SetNormalizedPosition(const Vector2& normalizedPosition);

		// The speed it goes on at (canvas units per second, in the viewport's space)
		const Vector2&		GetVelocity() const;
		void				StopMovement();

		// Emitted when the content moved, with the normalized position
		ScrolledEvent&		GetScrolledEvent();

		// Press (stops the movement), drag, scroll
		bool				OnPointerEvent(const PointerEvent& event) override;
		void				Update(float deltaTime) override;
		void				RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers) override;

	private:

		// The content and its viewport (its parent), both laid out, null if not set up so
		Node*				FindContent();
		// Content's rect in the viewport's space, from the last layout plus the offset moved since
		void				ComputeBounds(Node& content, Vector2& outContentMin, Vector2& outContentMax, Vector2& outViewMin, Vector2& outViewMax) const;
		// The move putting the content back within its edges (zero if it is): moved by it, it isn't past them anymore
		Vector2				ComputeCorrection(const Vector2& contentMin, const Vector2& contentMax, const Vector2& viewMin, const Vector2& viewMax) const;
		// A canvas space move into the viewport's space
		Vector2				ToViewport(Node& content, const Vector2& canvasDelta) const;
		Vector2				KeepAxes(const Vector2& vector) const;

	private:

		NodeRef<Node>		_content;
		bool				_horizontal = false;
		bool				_vertical = true;
		MovementType		_movementType = MovementType::Elastic;
		float				_elasticity = 0.1f;
		bool				_inertia = true;
		float				_decelerationRate = 0.135f;
		float				_scrollSensitivity = 50.0f;

		bool				_dragging = false;
		Vector2				_pendingDrag = Vector2::Zero;   // dragged since the last Update (viewport space)
		Vector2				_pendingScroll = Vector2::Zero; // wheel notches since the last Update
		Vector2				_velocity = Vector2::Zero;
		bool				_hasPendingPosition = false;
		Vector2				_pendingPosition = Vector2::Zero; // normalized
		Vector2				_movedSinceLayout = Vector2::Zero; // offset given since the content was last laid out
		Vector2				_laidOutPosition = Vector2::Zero;  // the content's position as last laid out (a change: laid out again)

		ScrolledEvent		_scrolledEvent;
	};
}
