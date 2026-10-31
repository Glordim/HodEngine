#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Math/Quaternion.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#include "HodEngine/UI2/Layout.hpp"
#include "HodEngine/UI2/Interaction.hpp"
#include "HodEngine/UI2/View.hpp"

namespace hod::inline core
{
	class Document;
	class DocumentNode;
}

namespace hod::inline ui2
{
	class LayoutParams;
	class DrawContext;
	class PrefabInstance;

	/// @brief
	class HOD_UI2_API Node
	{
		REFLECTED_CLASS_NO_PARENT(Node)

	public:

		using PropertyChangedEvent = Event<>;

	public:

		Node() = default;
		virtual ~Node();

		// Free-form label, not necessarily unique (siblings may share it): what the editor shows for
		// the node. Empty for a node saved before names existed.
		const String&				GetName() const;
		void						SetName(const String& name);

		// The descendant at path, names separated by '/' ("Panel/Title": the first child named Panel, then its first child named
		// Title), this node for an empty path. Goes through the nested prefab instances as through any node. nullptr if none.
		Node*						FindByPath(std::string_view path);
		template<typename _Node_>
		_Node_*						FindByPath(std::string_view path);

		// Identity local to the tree (a prefab) this node belongs to: what a serialized reference to
		// another node of the same tree must point at — unlike the name (editable, duplicable) or the
		// position in the tree (moves with edits). Same convention as the ECS: 0 means "not assigned
		// yet", and ids are never reused once given. They are taken from a counter belonging to the
		// persisted document, i.e. to whoever authors it (the prefab editor) — not to a Canvas, which
		// is only the runtime container a prefab gets instantiated into.
		uint64_t					GetLocalId() const;

		// Gives an id, taken from nextLocalId, to this node and its descendants that have none yet. Not to the content of the
		// prefab instances (see PrefabInstance), whose ids are their prefab's.
		void						AssignLocalIds(uint64_t& nextLocalId);
		// Gives this node and its descendants new ids, taken from nextLocalId, as a copy must not share its original's. The
		// references the subtree's Interactions and Views hold (see Interaction::RemapTargets, View::RemapReferences) follow for the nodes of the subtree; the
		// ones to nodes outside of it are kept if keepOutsideTargets, dropped otherwise (a copy pasted in another tree, where
		// those ids mean other nodes). The content of the prefab instances keeps its ids, its prefab's.
		void						RenewLocalIds(uint64_t& nextLocalId, bool keepOutsideTargets);

		// The node of that local id in the prefab whose scope this node roots (see GetScopeRoot): this node or, depth-first, one
		// of its descendants, without going into the nested prefab instances (see PrefabInstance), each a scope of its own whose
		// root only is found, by its id in this scope. This node, if it is an instance root, is found by its id in its own
		// prefab (PrefabInstance::GetRootLocalId). nullptr if there is none.
		Node*						FindByLocalId(uint64_t localId);
		// Where the local ids this node's references (e.g. its Interaction's overrides' targets) are resolved: the closest prefab
		// instance root among this node and its ancestors, the tree's root if none.
		Node*						GetScopeRoot();

		// Set on the root of a nested prefab's instance (see PrefabInstance), null on the other nodes.
		PrefabInstance*				GetPrefabInstance() const;
		// Turns this instance root's subtree into plain nodes of the containing prefab: they get new ids taken from nextLocalId
		// (theirs were their prefab's), which their Interactions' references follow. The nested instances it holds stay instances.
		void						UnpackPrefabInstance(uint64_t& nextLocalId);

		float						GetRotation() const;
		void						SetRotation(float rotation);
		//void						SetWorldRotation(float rotation);

		const Vector2&				GetScale() const;
		void						SetScale(const Vector2& scale);

		// Point of this node's own rect (0..1) around which its local rotation/scale is applied
		// (akin to CSS transform-origin). Purely visual, independent of where a LayoutParams puts
		// the node (e.g. AnchoredLayoutParams' Alignment): the two can differ on purpose.
		const Vector2&				GetPivot() const;
		void						SetPivot(const Vector2& pivot);

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
		// otherwise, if this node has children, measures each and returns the max of their sizes (an
		// implicit "ContentLayout"); otherwise returns MeasureContent(availableSize).
		//
		// Arrange: given the final rect this node's parent decided to grant it (in the PARENT's own
		// local space, i.e. relative to the parent's own origin), stores this node's resulting local
		// position/size and arranges children within its own content rect — via the attached Layout
		// if any, otherwise by letting each child's LayoutParams position it (e.g. AnchoredLayoutParams).
		Vector2						Measure(const Vector2& availableSize);
		void						Arrange(const Rect& finalRect);

		// Leaf content sizing hook: this node's own natural size when it has neither an attached
		// Layout nor children (e.g. TextNode's text bounds, ImageNode's texture size), used on the
		// axes its LayoutParams size from the content (LayoutParams::SizeMode::Content). Default
		// implementation returns zero: a plain node has no content of its own.
		virtual Vector2				MeasureContent(const Vector2& availableSize) const;

		// Cached result of the last Measure() call (the size this node wants, independent of the
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

		// Moves child (which must be one of this node's children) under newParent, which must not be
		// child itself nor one of its descendants. Its LayoutParams are kept (unlike RemoveChild +
		// AddChild, which would destroy them), converted if newParent expects another type.
		void						ReparentChild(Node* child, Node* newParent);

		// Puts replacement (a freshly created node of any Node type, parentless and childless) in this
		// node's place: it takes this node's base Node properties (name, local id — so references to the
		// node still resolve —, transform, z-order), its Layout, its children, its LayoutParams
		// and its place among its siblings. Properties specific to this node's own type are not carried over.
		// This node is left detached and empty, for the caller to delete.
		void						ReplaceWith(Node* replacement);
		// Moves child (one of this node's children) to index among its siblings, the last place if index is out of range:
		// drawn over the ones before it, placed after them by a Layout.
		void						SetChildIndex(Node* child, uint32_t index);

		// Whether ancestor is this node's parent, or the parent of its parent, and so on.
		bool						IsDescendantOf(const Node* ancestor) const;
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
		// Another type of layout keeping what it shares with the current one (e.g. a row turned into a column)
		void						ConvertLayout(const ReflectionDescriptor& layoutDescriptor);
		template<typename _Layout_>
		_Layout_*					GetLayout() const;
		Layout*						GetLayout() const;

		// Single, optional slot saying how THIS node reacts to the pointer (see Interaction), same ownership as the Layout:
		// SetInteraction<T>()/SetInteraction(Interaction*) destroy any previously attached one.
		template<typename _Interaction_, typename... Args>
		_Interaction_&				SetInteraction(Args&&... args);
		void						SetInteraction(Interaction* interaction);
		template<typename _Interaction_>
		_Interaction_*				GetInteraction() const;
		Interaction*				GetInteraction() const;

		// Single, optional slot for the code driving this node's prefab (see View), same ownership as the Layout: SetView<T>()/
		// SetView(View*) destroy any previously attached one (unbound first if it was bound).
		template<typename _View_, typename... Args>
		_View_&						SetView(Args&&... args);
		void						SetView(View* view);
		template<typename _View_>
		_View_*						GetView() const;
		View*						GetView() const;
		// A view of a type no loaded module registers (its game module missing): kept as read, to be saved back as is, until a
		// view is set (SetView) or it is dropped.
		bool						HasUnknownView() const;
		void						DropUnknownView();

		// Binds the views of this subtree (see View::Bind), the nested prefab instances' included: the deepest first, so a view
		// can use the views of the nodes below it once bound. UnbindViews unbinds them, the highest first.
		void						BindViews();
		void						UnbindViews();

		// Whether the hit-test (see HitTest) finds this node, catching the pointer: what is under it doesn't get it. Off for a
		// plain Node, on for the drawing ones filling their rect (Image, Shape). A node with an Interaction always catches the
		// pointer, whatever this says.
		bool						IsPointerTarget() const;
		void						SetPointerTarget(bool pointerTarget);

		// Deepest, topmost node of this subtree catching the pointer at canvasPosition (canvas space): children before their
		// parent and later siblings before earlier ones, i.e. the drawing order reversed. Within the node's rect as drawn
		// (pivot, rotation, scale applied), and, for the descendants of a node clipping its children (see ClipsChildren),
		// only within that node's rect too. Reads the cached layout results, so Canvas::UpdateLayout() must have run.
		Node*						HitTest(const Vector2& canvasPosition);

		PropertyChangedEvent&		GetPropertyChangedEvent();

		// Render pass: queues this node's own drawing (DrawSelf) then its children's (DrawChildren),
		// so the tree is drawn in painter's order — a node over its parent, later siblings over
		// earlier ones. Reads the cached layout results, so Canvas::UpdateLayout() must have run.
		void						PushRenderCommand(DrawContext& drawContext);

		// Serializes this node's own scalar properties, its attached Layout (if any, polymorphic),
		// its LayoutParams (polymorphic, resolved via LayoutParamsFactory), its concrete Node type
		// (resolved via NodeFactory, so drawing nodes survive a round trip) and its children tree,
		// recursively.
		bool						SerializeInDocument(DocumentNode& documentNode);
		bool						DeserializeFromDocument(const DocumentNode& documentNode);
		// One entry of SerializeInDocument's children: this node's concrete type, its LayoutParams (if it has some), then the
		// node itself and its subtree. Read back by AddChildFromDocument, e.g. to copy the subtree under another node.
		bool						SerializeAsChildInDocument(DocumentNode& documentNode);
		// Creates the node (and its subtree) an entry written by SerializeAsChildInDocument describes and adds it as this node's
		// last child. Its LayoutParams are kept if of the type this node expects, converted otherwise (see ReparentChild).
		// An instance of a prefab (see PrefabInstance) is built from the prefab, then given its overrides.
		// @return the new child, nullptr if the entry couldn't be read (nothing is added then)
		Node*						AddChildFromDocument(const DocumentNode& documentNode);

	public:

		// Computes the aligned+clamped [offset, extent] of a child along one axis. alignIndex 0/2 are
		// the two "edge" alignments (startIsIndex0 says which one sits at availableStart), 1 is
		// centered, 3 stretches to fill availableExtent. VerticalLayoutParams::HorizontalAlign and HorizontalLayoutParams::VerticalAlign
		// share this same 4-value layout (Stretch is always index 3), so both can drive this helper.
		// Public and static (no instance state) so a Layout implementation can reuse it for its own
		// cross-axis placement (see BoxLayout).
		static void					ComputeAlignedExtent(
										float availableStart, float availableExtent, float desiredExtent, float minExtent, float maxExtent, uint8_t alignIndex, bool startIsIndex0, float& outOffset, float& outExtent);

	protected:

		// What a drawing node overrides (ImageNode, TextNode...). Default: draws nothing.
		virtual void				DrawSelf(DrawContext& drawContext);

		// Draws the children in order. A node wrapping them in some render state (MaskNode's clip)
		// overrides this and calls Node::DrawChildren() in between setting and restoring it.
		virtual void				DrawChildren(DrawContext& drawContext);

		// Whether nothing of the children shows outside of this node's rect (MaskNode): the hit-test doesn't find them there either.
		virtual bool				ClipsChildren() const;

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
		// available, otherwise stretches the child over parentContentRect (within its min/max size).
		// This is Node's default (content-layout) per-child placement, used only when
		// no Layout is attached.
		static void					ArrangeChildInRect(Node* child, const Rect& parentContentRect);

		// FindByLocalId's search below the scope root: an instance root is found by its id in the containing prefab, and closes its subtree
		Node*						FindInScope(uint64_t localId);
		// The LayoutParams an entry (see SerializeAsChildInDocument) gives, of the type this node expects
		LayoutParams*				ReadLayoutParams(const DocumentNode& entry) const;
		Node*						AddPrefabInstanceFromDocument(const DocumentNode& entry, const DocumentNode& instanceNode);
		bool						SerializePrefabInstance(DocumentNode& documentNode);

	private:

		String						_name;
		uint64_t					_localId = 0;

		int32_t						_zOrder = std::numeric_limits<int32_t>().lowest();
		int32_t						_globalZOrder = 0;

		float						_rotation = 0.0f;
		Vector2						_scale = Vector2::One;
		Vector2						_pivot = Vector2(0.5f, 0.5f);

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

		// Single, optional, owned (see SetInteraction above).
		Interaction*				_interaction = nullptr;
		bool						_pointerTarget = false;

		// Single, optional, owned (see SetView above). _unknownView: owned, see HasUnknownView.
		View*						_view = nullptr;
		Document*					_unknownView = nullptr;

		// Owned, see GetPrefabInstance
		PrefabInstance*				_prefabInstance = nullptr;
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

	template<typename _Interaction_, typename... Args>
	_Interaction_& Node::SetInteraction(Args&&... args)
	{
		static_assert(std::is_base_of<Interaction, _Interaction_>::value, "_Interaction_ must derive from Interaction to use Node::SetInteraction()");

		_Interaction_* interaction = DefaultAllocator::GetInstance().New<_Interaction_>(std::forward<Args>(args)...);
		SetInteraction(interaction);
		return *interaction;
	}

	template<typename _Interaction_>
	_Interaction_* Node::GetInteraction() const
	{
		return Interaction::Cast<_Interaction_>(GetInteraction());
	}

	template<typename _View_, typename... Args>
	_View_& Node::SetView(Args&&... args)
	{
		static_assert(std::is_base_of<View, _View_>::value, "_View_ must derive from View to use Node::SetView()");

		_View_* view = DefaultAllocator::GetInstance().New<_View_>(std::forward<Args>(args)...);
		SetView(view);
		return *view;
	}

	template<typename _View_>
	_View_* Node::GetView() const
	{
		return View::Cast<_View_>(GetView());
	}

	/// @brief nullptr also when the node there isn't a _Node_
	template<typename _Node_>
	_Node_* Node::FindByPath(std::string_view path)
	{
		static_assert(std::is_base_of<Node, _Node_>::value, "_Node_ must derive from Node to use Node::FindByPath()");

		Node* node = FindByPath(path);
		if (node != nullptr && node->GetReflectionDescriptorV().IsCompatible(_Node_::GetReflectionDescriptor()))
		{
			return static_cast<_Node_*>(node);
		}
		return nullptr;
	}
}
