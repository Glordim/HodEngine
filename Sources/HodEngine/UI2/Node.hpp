#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Math/Quaternion.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#include "HodEngine/UI2/Layout.hpp"

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline ui2
{
	class LayoutParams;

	/// @brief
	class HOD_UI2_API Node
	{
		REFLECTED_CLASS_NO_PARENT(Node)

	public:

		using PropertyChangedEvent = Event<>;

	public:

		Node() = default;
		virtual ~Node();

		float						GetRotation() const;
		void						SetRotation(float rotation);
		//void						SetWorldRotation(float rotation);

		const Vector2&				GetScale() const;
		void						SetScale(const Vector2& scale);

		// Point of this node's own rect around which its local rotation/scale is applied
		// (akin to CSS transform-origin). Independent of any anchor pivot a LayoutParams may
		// define for positioning: the two can differ on purpose.
		const Vector2&				GetOrigin() const;
		void						SetOrigin(const Vector2& origin);

		const Vector2&				GetDesiredSize() const;
		void						SetDesiredSize(const Vector2& desiredSize);

		void						SetZOrder(int32_t zOrder);
		int32_t						GetZOrder() const;
		int32_t						ComputeZOrder();

		void						MarkMeasureAsDirty();
		void						MarkArrangeAsDirty();

		// Top-down layout pass. Never called directly except from Canvas::UpdateLayout(); everything
		// else should read the cached GetSize()/GetLocalMatrix()/ComputeCanvasMatrix() results below.
		// Not virtual: a widget customizes arrangement via SetLayout() (see below) and intrinsic leaf
		// sizing via MeasureContent(), not by overriding the two-pass protocol itself.
		//
		// Measure: given the space this node's parent is willing to offer, returns (and caches as
		// GetMeasuredSize()) the size this node wants. When a Layout is attached, delegates to it;
		// otherwise, if this node has children, measures each and returns the max of their margined
		// sizes (an implicit "ContentLayout"); otherwise returns MeasureContent(availableSize).
		//
		// Arrange: given the final rect this node's parent decided to grant it (in the PARENT's own
		// local space, i.e. relative to the parent's own origin), stores this node's resulting local
		// position/size and arranges children within its own content rect — via the attached Layout
		// if any, otherwise per each child's LayoutParams margin/HAlign/VAlign (or lets the
		// LayoutParams position it outright, e.g. AnchoredLayoutParams).
		Vector2						Measure(const Vector2& availableSize);
		void						Arrange(const Rect& finalRect);

		// Leaf content sizing hook: this node's own natural size when it has neither an attached
		// Layout nor children (e.g. a future TextNode measuring its own text bounds). Default
		// implementation returns GetDesiredSize().
		virtual Vector2				MeasureContent(const Vector2& availableSize) const;

		// Cached result of the last Measure() call (this node's desired size, independent of the
		// final size Arrange may actually grant it — e.g. a Stretch-aligned child's GetSize() can
		// end up larger than its GetMeasuredSize()).
		const Vector2&				GetMeasuredSize() const;

		const Vector2&				GetSize() const;
		const Vector2&				GetPosition() const;
		const Matrix4&				GetLocalMatrix() const;

		const Matrix4&				ComputeCanvasMatrix();
		Matrix4						ComputeWorldMatrix();

		// Convenience overload: places child using CreateDefaultLayoutParams(). Prefer the
		// explicit overload below when the child needs a specific, non-default arrangement.
		void						AddChild(Node* child);

		// child must not be null; layoutParams must not be null and must have been freshly
		// allocated for this placement (ownership transfers to this Node, see _layoutParams).
		void						AddChild(Node* child, LayoutParams* layoutParams);
		void						RemoveChild(Node* child);
		Node*						GetParent() const;
		const Vector<Node*>&		GetChildren() const;

		LayoutParams*				GetLayoutParams() const;

		// LayoutParams type a child added via the no-argument AddChild() should receive: the
		// attached Layout's choice if one is set (see SetLayout below), otherwise AnchoredLayoutParams.
		LayoutParams*				CreateDefaultLayoutParams() const;

		// Single, optional arrangement-algorithm slot governing how THIS node's children are
		// arranged (see the Layout class comment) — never a collection, at most one at a time.
		// Owned by this Node: SetLayout<T>()/SetLayout(Layout*) destroys any previously attached
		// Layout. No Layout attached means "use the built-in default content placement", not
		// "arrange nothing".
		template<typename _Layout_, typename... Args>
		_Layout_&					SetLayout(Args&&... args);
		void						SetLayout(Layout* layout);
		template<typename _Layout_>
		_Layout_*					GetLayout() const;
		Layout*						GetLayout() const;

		PropertyChangedEvent&		GetPropertyChangedEvent();

		// Serializes this node's own scalar properties, its attached Layout (if any, polymorphic),
		// its LayoutParams (polymorphic, resolved via LayoutParamsFactory), and its children tree,
		// recursively.
		bool						SerializeInDocument(DocumentNode& documentNode);
		bool						DeserializeFromDocument(const DocumentNode& documentNode);

	public:

		// Computes the aligned+clamped [offset, extent] of a child along one axis. alignIndex 0/2 are
		// the two "edge" alignments (startIsIndex0 says which one sits at availableStart), 1 is
		// centered, 3 stretches to fill availableExtent. LayoutParams::HAlign and LayoutParams::VAlign
		// share this same 4-value layout (Stretch is always index 3), so both can drive this helper.
		// Public and static (no instance state) so a Layout implementation can reuse it for its own
		// cross-axis placement (see BoxLayout).
		static void					ComputeAlignedExtent(
										float availableStart, float availableExtent, float desiredExtent, float minExtent, float maxExtent, uint8_t alignIndex, bool startIsIndex0, float& outOffset, float& outExtent);

	private:

		enum class DirtyFlag : uint8_t
		{
			Measure = (1 << 0),
			Arrange = (1 << 1),
			CanvasMatrix = (1 << 2),
			ZOrder = (1 << 3),
		};

	private:

		bool						NeedsMeasure() const;
		bool						NeedsArrange() const;
		void						FinishMeasure(const Vector2& measuredSize);
		Rect						BeginArrange(const Rect& finalRect);
		Vector2						ResolveMeasuredSize(const Vector2& availableSize, const Vector2& contentDerivedSize) const;

		void						ComputeLocalMatrix(const Vector2& position, const Vector2& size, Matrix4& localMatrix) const;
		void						MarkCanvasMatrixAsDirty();

		// Places one child within parentContentRect (expressed in this node's own local space):
		// defers to the child's LayoutParams::TryComputePosition (e.g. AnchoredLayoutParams) when
		// available, otherwise falls back to generic margin/HAlign/VAlign placement of the child's
		// measured size. This is Node's default (content-layout) per-child placement, used only when
		// no Layout is attached.
		static void					ArrangeChildInRect(Node* child, const Rect& parentContentRect);

	private:

		int32_t						_zOrder = std::numeric_limits<int32_t>().lowest();
		int32_t						_globalZOrder = 0;

		float						_rotation = 0.0f;
		Vector2						_scale = Vector2::One;
		Vector2						_origin = Vector2(0.5f, 0.5f);

		Vector2						_desiredSize = Vector2(100.0f, 100.0f);

		Vector2						_measuredSize = Vector2::Zero;

		Vector2						_size = Vector2::Zero;
		Vector2						_position = Vector2::Zero;
		Matrix4						_localMatrix = Matrix4::Identity;
		Matrix4						_canvasMatrix = Matrix4::Identity;
		uint8_t						_dirtyFlags = (uint8_t)DirtyFlag::Measure | (uint8_t)DirtyFlag::Arrange | (uint8_t)DirtyFlag::CanvasMatrix | (uint8_t)DirtyFlag::ZOrder;

		Node*						_parent = nullptr;

		// Non-owning cache of this node's own layout parameters, for O(1) access during layout
		// computation. Ownership stays with the parent that provided it in AddChild: the parent
		// allocates it, and this Node's parent is responsible for deleting it (see
		// Node::RemoveChild / Node::~Node), never this Node itself directly.
		LayoutParams*				_layoutParams = nullptr;

		// Single, optional, owned arrangement-algorithm attached to THIS node (governs how ITS OWN
		// children are arranged) — see Layout and SetLayout above. Distinct from _layoutParams, which
		// describes how THIS node is placed within its PARENT.
		Layout*						_layout = nullptr;

		Vector<Node*>				_children;

		PropertyChangedEvent		_propertyChangedEvent;
	};

	template<typename _Layout_, typename... Args>
	_Layout_& Node::SetLayout(Args&&... args)
	{
		static_assert(std::is_base_of<Layout, _Layout_>::value, "_Layout_ must derive from Layout to use Node::SetLayout()");

		_Layout_* layout = DefaultAllocator::GetInstance().New<_Layout_>(std::forward<Args>(args)...);
		SetLayout(layout);
		return *layout;
	}

	template<typename _Layout_>
	_Layout_* Node::GetLayout() const
	{
		return Layout::Cast<_Layout_>(GetLayout());
	}
}
