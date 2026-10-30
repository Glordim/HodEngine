#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Assert.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"
#include "HodEngine/UI2/LayoutParamsFactory.hpp"
#include "HodEngine/UI2/Layout.hpp"
#include "HodEngine/UI2/LayoutFactory.hpp"
#include "HodEngine/UI2/InteractionFactory.hpp"
#include "HodEngine/UI2/NodeFactory.hpp"
#include "HodEngine/UI2/DrawContext.hpp"
#include "HodEngine/UI2/PrefabInstance.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <algorithm>
#include <cmath>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(Node, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_VECTOR_SQUARE);

		// Serialized like any property, but never edited by hand: hidden from the inspector (same as the ECS' _localId).
		AddPropertyT(reflectionDescriptor, &Node::_localId, "_localId")->AddTrait<ReflectionTraitHide>();

		AddPropertyT(reflectionDescriptor, &Node::_name, "Name", &Node::SetName);

		AddPropertyT(reflectionDescriptor, &Node::_rotation, "Rotation", &Node::SetRotation);
		AddPropertyT(reflectionDescriptor, &Node::_scale, "Scale", &Node::SetScale);
		AddPropertyT(reflectionDescriptor, &Node::_pivot, "Pivot", &Node::SetPivot);

		AddPropertyT(reflectionDescriptor, &Node::_pointerTarget, "PointerTarget", &Node::SetPointerTarget);
	}

	namespace
	{
		/// @brief Copies, from source to destination, the reflected properties of every type both derive from: those of their
		/// closest common ancestor and of its own ancestors (e.g. BoxLayoutParams' and LayoutParams' between a HorizontalLayoutParams
		/// and a VerticalLayoutParams, only LayoutParams' between an AnchoredLayoutParams and either). Straight copies, no setter called.
		void CopyCommonProperties(const ReflectionDescriptor& sourceDescriptor, const void* source, const ReflectionDescriptor& destinationDescriptor, void* destination)
		{
			const ReflectionDescriptor* commonDescriptor = &sourceDescriptor;
			while (commonDescriptor != nullptr && destinationDescriptor.IsCompatible(*commonDescriptor) == false)
			{
				commonDescriptor = commonDescriptor->GetParent();
			}

			for (; commonDescriptor != nullptr; commonDescriptor = commonDescriptor->GetParent())
			{
				for (ReflectionProperty* property : commonDescriptor->GetProperties())
				{
					property->Copy(source, destination);
				}
			}
		}

		/// @brief Which LayoutParams a child must end up with, given the one it has (may be null) and a freshly
		/// created default one of the type its (new) parent expects: the current one if it already is of that
		/// type, otherwise the fresh one, carrying over what the two types share (see CopyCommonProperties and
		/// LayoutParams::CarryOverFrom). Destroys whichever of the two is not returned.
		LayoutParams* AdaptLayoutParams(LayoutParams* current, LayoutParams* freshDefault)
		{
			if (current != nullptr && current->GetReflectionDescriptorV().GetType() == freshDefault->GetReflectionDescriptorV().GetType())
			{
				DefaultAllocator::GetInstance().Delete(freshDefault);
				return current;
			}

			if (current != nullptr)
			{
				CopyCommonProperties(current->GetReflectionDescriptorV(), current, freshDefault->GetReflectionDescriptorV(), freshDefault);
				freshDefault->CarryOverFrom(*current);
				DefaultAllocator::GetInstance().Delete(current);
			}
			return freshDefault;
		}
	}

	Node::~Node()
	{
		DefaultAllocator::GetInstance().Delete(_layout);
		DefaultAllocator::GetInstance().Delete(_interaction);
		DefaultAllocator::GetInstance().Delete(_prefabInstance);

		for (Node* child : _children)
		{
			DefaultAllocator::GetInstance().Delete(child->_layoutParams);
			DefaultAllocator::GetInstance().Delete(child);
		}
	}

	/// @brief
	/// @return
	const String& Node::GetName() const
	{
		return _name;
	}

	/// @brief
	/// @param name
	void Node::SetName(const String& name)
	{
		if (_name != name)
		{
			_name = name;
			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	uint64_t Node::GetLocalId() const
	{
		return _localId;
	}

	/// @brief
	/// @param nextLocalId
	void Node::AssignLocalIds(uint64_t& nextLocalId)
	{
		if (_localId == 0)
		{
			_localId = nextLocalId;
			++nextLocalId;
		}

		if (_prefabInstance != nullptr)
		{
			return; // Its content has its prefab's ids
		}

		for (Node* child : _children)
		{
			child->AssignLocalIds(nextLocalId);
		}
	}

	/// @brief
	/// @param nextLocalId
	/// @param keepOutsideTargets
	void Node::RenewLocalIds(uint64_t& nextLocalId, bool keepOutsideTargets)
	{
		std::map<uint64_t, uint64_t> oldToNew;
		Vector<Node*>                nodes = {this};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			Node* node = nodes[index];
			if (node->_localId != 0)
			{
				oldToNew[node->_localId] = nextLocalId;
			}
			node->_localId = nextLocalId;
			++nextLocalId;

			// An instance's content keeps its prefab's ids
			if (node->_prefabInstance == nullptr)
			{
				for (Node* child : node->_children)
				{
					nodes.PushBack(child);
				}
			}
		}

		// Once every id of the subtree is known: an Interaction may reference a node visited after its own. Not an instance
		// root's, which references nodes of its prefab.
		for (Node* node : nodes)
		{
			if (node->_interaction != nullptr && node->_prefabInstance == nullptr)
			{
				node->_interaction->RemapTargets(oldToNew, keepOutsideTargets);
			}
		}
	}

	/// @brief
	/// @param localId
	/// @return
	Node* Node::FindByLocalId(uint64_t localId)
	{
		if (localId == 0)
		{
			return nullptr;
		}

		uint64_t scopeLocalId = (_prefabInstance != nullptr) ? _prefabInstance->GetRootLocalId() : _localId;
		if (scopeLocalId == localId)
		{
			return this;
		}

		for (Node* child : _children)
		{
			Node* found = child->FindInScope(localId);
			if (found != nullptr)
			{
				return found;
			}
		}

		return nullptr;
	}

	/// @brief
	/// @param localId
	/// @return
	Node* Node::FindInScope(uint64_t localId)
	{
		if (_localId == localId)
		{
			return this;
		}

		if (_prefabInstance != nullptr)
		{
			return nullptr;
		}

		for (Node* child : _children)
		{
			Node* found = child->FindInScope(localId);
			if (found != nullptr)
			{
				return found;
			}
		}

		return nullptr;
	}

	/// @brief
	/// @return
	Node* Node::GetScopeRoot()
	{
		Node* node = this;
		while (node->_prefabInstance == nullptr && node->_parent != nullptr)
		{
			node = node->_parent;
		}
		return node;
	}

	/// @brief
	/// @return
	PrefabInstance* Node::GetPrefabInstance() const
	{
		return _prefabInstance;
	}

	/// @brief
	/// @param nextLocalId
	void Node::UnpackPrefabInstance(uint64_t& nextLocalId)
	{
		if (_prefabInstance == nullptr)
		{
			return;
		}

		// From the ids in the prefab to the new ones in the containing prefab: the root keeps the one it has there
		std::map<uint64_t, uint64_t> prefabToNew;
		if (_prefabInstance->GetRootLocalId() != 0)
		{
			prefabToNew[_prefabInstance->GetRootLocalId()] = _localId;
		}

		Vector<Node*> nodes(_children);
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			Node* node = nodes[index];
			if (node->_localId != 0)
			{
				prefabToNew[node->_localId] = nextLocalId;
			}
			node->_localId = nextLocalId;
			++nextLocalId;

			// A nested instance stays one, with its own ids
			if (node->_prefabInstance == nullptr)
			{
				for (Node* child : node->_children)
				{
					nodes.PushBack(child);
				}
			}
		}

		// The prefab's Interactions only referenced its own nodes
		if (_interaction != nullptr)
		{
			_interaction->RemapTargets(prefabToNew, false);
		}
		for (Node* node : nodes)
		{
			if (node->_interaction != nullptr && node->_prefabInstance == nullptr)
			{
				node->_interaction->RemapTargets(prefabToNew, false);
			}
		}

		DefaultAllocator::GetInstance().Delete(_prefabInstance);
		_prefabInstance = nullptr;
	}

	void Node::AddChild(Node* child)
	{
		AddChild(child, CreateDefaultLayoutParams());
	}

	void Node::AddChild(Node* child, LayoutParams* layoutParams)
	{
		Assert(child != nullptr);
		Assert(layoutParams != nullptr);

		child->_parent = this;
		child->_layoutParams = layoutParams;
		layoutParams->_node = child;

		_children.PushBack(child);

		// child->MarkMeasureAsDirty() alone isn't enough: a freshly constructed Node already starts
		// with its Measure flag set (see _dirtyFlags' default), so that call's own dirty check finds
		// it already dirty and never walks up to notify `this` — which just gained a new child and
		// needs to know regardless of child's prior state (also covers re-parenting an already-clean
		// node, which still needs `this` in particular, not just its old parent, marked dirty).
		child->MarkMeasureAsDirty();
		MarkMeasureAsDirty();
	}

	void Node::RemoveChild(Node* child)
	{
		auto it = std::find(_children.Begin(), _children.End(), child);
		if (it != _children.End())
		{
			DefaultAllocator::GetInstance().Delete(child->_layoutParams);
			child->_layoutParams = nullptr;
			child->_parent = nullptr;

			_children.Erase(it);

			MarkMeasureAsDirty();
		}
	}

	void Node::ReparentChild(Node* child, Node* newParent)
	{
		Assert(newParent != nullptr);
		Assert(newParent != this);
		Assert(newParent != child && newParent->IsDescendantOf(child) == false);

		auto it = std::find(_children.Begin(), _children.End(), child);
		if (it == _children.End())
		{
			return;
		}

		// The new parent may expect another LayoutParams type than this one does (e.g. a BoxLayout).
		LayoutParams* layoutParams = AdaptLayoutParams(child->_layoutParams, newParent->CreateDefaultLayoutParams());
		child->_layoutParams = nullptr;
		child->_parent = nullptr;

		_children.Erase(it);
		MarkMeasureAsDirty();

		newParent->AddChild(child, layoutParams);
	}

	void Node::SetChildIndex(Node* child, uint32_t index)
	{
		auto it = std::find(_children.Begin(), _children.End(), child);
		if (it == _children.End())
		{
			return;
		}

		_children.Erase(it);
		_children.Insert(Min(index, _children.Size()), child);

		MarkMeasureAsDirty();
	}

	void Node::ReplaceWith(Node* replacement)
	{
		Assert(replacement != nullptr && replacement != this);
		Assert(replacement->_parent == nullptr && replacement->_children.Empty());

		replacement->_name = _name;
		replacement->_localId = _localId;
		replacement->_zOrder = _zOrder;
		replacement->_rotation = _rotation;
		replacement->_scale = _scale;
		replacement->_pivot = _pivot;
		// _pointerTarget not carried over: its default depends on the node type (a plain Node turned into an Image catches the pointer).

		// Moved as is rather than through SetLayout: the children's LayoutParams already are of the type it expects.
		DefaultAllocator::GetInstance().Delete(replacement->_layout);
		replacement->_layout = _layout;
		if (_layout != nullptr)
		{
			_layout->_node = replacement;
			_layout = nullptr;
		}

		DefaultAllocator::GetInstance().Delete(replacement->_interaction);
		replacement->_interaction = _interaction;
		if (_interaction != nullptr)
		{
			_interaction->_node = replacement;
			_interaction = nullptr;
		}

		replacement->_children = std::move(_children);
		_children.Clear();
		for (Node* child : replacement->_children)
		{
			child->_parent = replacement;
		}

		if (_parent != nullptr)
		{
			auto it = std::find(_parent->_children.Begin(), _parent->_children.End(), this);
			Assert(it != _parent->_children.End());
			*it = replacement;

			replacement->_parent = _parent;
			replacement->_layoutParams = _layoutParams;
			if (_layoutParams != nullptr)
			{
				_layoutParams->_node = replacement;
			}

			_parent = nullptr;
			_layoutParams = nullptr;
		}

		// Same as AddChild: the fresh replacement already starts dirty, so its own call would not reach the parent.
		replacement->MarkMeasureAsDirty();
		if (replacement->_parent != nullptr)
		{
			replacement->_parent->MarkMeasureAsDirty();
		}
	}

	bool Node::IsDescendantOf(const Node* ancestor) const
	{
		for (const Node* node = _parent; node != nullptr; node = node->_parent)
		{
			if (node == ancestor)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @return
	float Node::GetRotation() const
	{
		return _rotation;
	}

	/// @brief
	/// @param rotation
	void Node::SetRotation(float rotation)
	{
		if (_rotation != rotation)
		{
			_rotation = rotation;
			MarkArrangeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& Node::GetScale() const
	{
		return _scale;
	}

	/// @brief
	/// @param scale
	void Node::SetScale(const Vector2& scale)
	{
		if (_scale != scale)
		{
			_scale = scale;
			MarkArrangeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& Node::GetPivot() const
	{
		return _pivot;
	}

	/// @brief
	/// @param pivot
	void Node::SetPivot(const Vector2& pivot)
	{
		if (_pivot != pivot)
		{
			_pivot = pivot;
			MarkArrangeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	void Node::MarkMeasureAsDirty()
	{
		if ((_dirtyFlags & (uint8_t)DirtyFlag::Measure) == 0)
		{
			_dirtyFlags |= (uint8_t)DirtyFlag::Measure;

			if (_parent != nullptr)
			{
				_parent->MarkMeasureAsDirty();
			}
		}

		// A new measure means a new arrange, even when the measured size ends up unchanged (FinishMeasure
		// only flags Arrange on a size change): e.g. a child added to an already laid out node must still be
		// reached by the arrange pass. Outside the check above for the same reason as in AddChild.
		MarkArrangeAsDirty();
	}

	/// @brief
	void Node::MarkArrangeAsDirty()
	{
		if ((_dirtyFlags & (uint8_t)DirtyFlag::Arrange) == 0)
		{
			_dirtyFlags |= (uint8_t)DirtyFlag::Arrange;

			if (_parent != nullptr)
			{
				_parent->MarkArrangeAsDirty();
			}
		}
	}

	/// @brief
	void Node::MarkCanvasMatrixAsDirty()
	{
		if ((_dirtyFlags & (uint8_t)DirtyFlag::CanvasMatrix) == 0)
		{
			_dirtyFlags |= (uint8_t)DirtyFlag::CanvasMatrix;
			for (uint32_t childIndex = 0; childIndex < _children.Size(); ++childIndex)
			{
				Node* childNode = _children[childIndex];
				if (childNode != nullptr)
				{
					childNode->MarkCanvasMatrixAsDirty();
				}
			}
		}
	}

	/// @brief
	/// @return
	bool Node::NeedsMeasure() const
	{
		return (_dirtyFlags & (uint8_t)DirtyFlag::Measure) != 0 || (_layoutParams != nullptr && _layoutParams->DependsOnParentSize());
	}

	/// @brief
	/// @return
	bool Node::NeedsArrange() const
	{
		return (_dirtyFlags & (uint8_t)DirtyFlag::Arrange) != 0 || (_layoutParams != nullptr && _layoutParams->DependsOnParentSize());
	}

	/// @brief
	/// @param measuredSize
	void Node::FinishMeasure(const Vector2& measuredSize)
	{
		Vector2 previousMeasuredSize = _measuredSize;
		_measuredSize = measuredSize;

		if (_measuredSize != previousMeasuredSize)
		{
			MarkArrangeAsDirty();
		}

		_dirtyFlags &= ~((uint8_t)DirtyFlag::Measure);
	}

	/// @brief
	/// @param finalRect
	/// @return
	Rect Node::BeginArrange(const Rect& finalRect)
	{
		_size = finalRect._size;
		_position = finalRect._position + finalRect._size * 0.5f;

		ComputeLocalMatrix(_position, _size, _localMatrix);
		MarkCanvasMatrixAsDirty();

		_dirtyFlags &= ~((uint8_t)DirtyFlag::Arrange);

		Rect contentRect;
		contentRect._position = _size * -0.5f;
		contentRect._size = _size;
		return contentRect;
	}

	/// @brief
	/// @param availableSize
	/// @return
	Vector2 Node::Measure(const Vector2& availableSize)
	{
		if (NeedsMeasure() == false)
		{
			return _measuredSize;
		}

		Vector2 contentDerivedSize;
		if (_layout != nullptr)
		{
			contentDerivedSize = _layout->Measure(*this, availableSize);
		}
		else if (_children.Empty() == false)
		{
			Vector2 maxChildSize = Vector2::Zero;
			for (Node* child : _children)
			{
				if (child == nullptr)
				{
					continue;
				}

				Vector2 childSize = child->Measure(availableSize);

				// A child sized from this node on an axis (stretched anchors) can't size it back: only its MinSize counts there,
				// it spans whatever the other children give once arranged (see ArrangeChildInRect).
				if (const LayoutParams* childLayoutParams = child->_layoutParams)
				{
					if (childLayoutParams->DependsOnParentWidth())
					{
						childSize.SetX(childLayoutParams->GetMinSize().GetX());
					}
					if (childLayoutParams->DependsOnParentHeight())
					{
						childSize.SetY(childLayoutParams->GetMinSize().GetY());
					}
				}

				maxChildSize = Vector2(std::max(maxChildSize.GetX(), childSize.GetX()), std::max(maxChildSize.GetY(), childSize.GetY()));
			}

			contentDerivedSize = maxChildSize;
		}
		else
		{
			contentDerivedSize = MeasureContent(availableSize);
		}

		FinishMeasure(ResolveMeasuredSize(availableSize, contentDerivedSize));
		return _measuredSize;
	}

	/// @brief
	/// @param availableSize
	/// @param contentDerivedSize
	/// @return
	Vector2 Node::ResolveMeasuredSize(const Vector2& availableSize, const Vector2& contentDerivedSize) const
	{
		// Without LayoutParams (the canvas' root), the node is whatever its parent grants it at Arrange.
		if (_layoutParams == nullptr)
		{
			return contentDerivedSize;
		}
		return _layoutParams->ComputeSize(availableSize, contentDerivedSize);
	}

	/// @brief
	/// @param availableSize
	/// @return
	Vector2 Node::MeasureContent(const Vector2& availableSize) const
	{
		(void)availableSize;
		return Vector2::Zero;
	}

	/// @brief
	/// @param finalRect
	void Node::Arrange(const Rect& finalRect)
	{
		// A granted rect that moved is arranged again even when nothing is dirty: e.g. a node anchored on
		// a single point (not DependsOnParentSize) moves when its parent is resized.
		bool rectChanged = finalRect._size != _size || finalRect._position + finalRect._size * 0.5f != _position;
		if (NeedsArrange() == false && rectChanged == false)
		{
			return;
		}

		Rect contentRect = BeginArrange(finalRect);

		if (_layout != nullptr)
		{
			_layout->Arrange(*this, contentRect);
		}
		else
		{
			for (Node* child : _children)
			{
				if (child != nullptr)
				{
					ArrangeChildInRect(child, contentRect);
				}
			}
		}
	}

	/// @brief
	/// @param child
	/// @param parentContentRect
	void Node::ArrangeChildInRect(Node* child, const Rect& parentContentRect)
	{
		LayoutParams* childLayoutParams = child->_layoutParams;

		Vector2 overriddenPosition;
		// Measure only offered the child the space its parent was offered, not the parent's actual rect:
		// a child sized from its parent's (e.g. stretched anchors) is measured again against the real one.
		if (childLayoutParams != nullptr && childLayoutParams->DependsOnParentSize())
		{
			child->Measure(parentContentRect._size);
		}

		if (childLayoutParams != nullptr && childLayoutParams->TryComputePosition(parentContentRect, child->_measuredSize, overriddenPosition))
		{
			Rect childFinalRect;
			childFinalRect._size = child->_measuredSize;
			childFinalRect._position = overriddenPosition - child->_measuredSize * 0.5f;
			child->Arrange(childFinalRect);
			return;
		}

		// LayoutParams that don't place their child: stretched over the parent's content rect, within its min/max size
		constexpr uint8_t stretchIndex = 3;
		Vector2           minSize = childLayoutParams != nullptr ? childLayoutParams->GetMinSize() : Vector2::Zero;
		Vector2           maxSize = childLayoutParams != nullptr ? childLayoutParams->GetMaxSize() : Vector2::Infinity;

		float x, width, y, height;
		ComputeAlignedExtent(parentContentRect._position.GetX(), parentContentRect._size.GetX(), child->_measuredSize.GetX(), minSize.GetX(), maxSize.GetX(), stretchIndex, true, x,
		                     width);
		ComputeAlignedExtent(parentContentRect._position.GetY(), parentContentRect._size.GetY(), child->_measuredSize.GetY(), minSize.GetY(), maxSize.GetY(), stretchIndex, false, y,
		                     height);

		Rect childFinalRect;
		childFinalRect._position = Vector2(x, y);
		childFinalRect._size = Vector2(width, height);

		child->Arrange(childFinalRect);
	}

	/// @brief
	/// @param availableStart
	/// @param availableExtent
	/// @param desiredExtent
	/// @param minExtent
	/// @param maxExtent
	/// @param alignIndex
	/// @param startIsIndex0
	/// @param outOffset
	/// @param outExtent
	void Node::ComputeAlignedExtent(
		float availableStart, float availableExtent, float desiredExtent, float minExtent, float maxExtent, uint8_t alignIndex, bool startIsIndex0, float& outOffset, float& outExtent)
	{
		constexpr uint8_t centerIndex = 1;
		constexpr uint8_t stretchIndex = 3;

		if (alignIndex == stretchIndex)
		{
			outExtent = std::clamp(availableExtent, minExtent, maxExtent);
			outOffset = availableStart;
			return;
		}

		outExtent = std::clamp(desiredExtent, minExtent, maxExtent);

		if (alignIndex == centerIndex)
		{
			outOffset = availableStart + (availableExtent - outExtent) * 0.5f;
			return;
		}

		bool isStartSide = (alignIndex == 0) ? startIsIndex0 : (startIsIndex0 == false);
		outOffset = isStartSide ? availableStart : (availableStart + availableExtent - outExtent);
	}

	/// @brief
	/// @return
	const Vector2& Node::GetMeasuredSize() const
	{
		return _measuredSize;
	}

	/// @brief
	/// @return
	const Vector2& Node::GetSize() const
	{
		return _size;
	}

	/// @brief
	/// @return
	const Vector2& Node::GetPosition() const
	{
		return _position;
	}

	/// @brief
	/// @return
	const Matrix4& Node::GetLocalMatrix() const
	{
		return _localMatrix;
	}

	/// @brief
	/// @param position
	/// @param size
	/// @param localMatrix
	void Node::ComputeLocalMatrix(const Vector2& position, const Vector2& size, Matrix4& localMatrix) const
	{
		const Vector2& pixelPivot = (_pivot - Vector2(0.5f, 0.5f)) * size;

		Matrix4 toPivot = Matrix4::Translation(pixelPivot);
		Matrix4 fromPivot = Matrix4::Translation(-pixelPivot);

		Matrix4 translation = Matrix4::Translation(position);
		Matrix4 rotation = Matrix4::Rotation(_rotation);
		Matrix4 scaling = Matrix4::Scale(_scale);

		// Column vectors (M * v, the rightmost matrix applies first): scaled then rotated around the pivot point, then moved to the node's center.
		// Purely visual: the layout (sizes, positions) never sees the scale or the rotation.
		localMatrix = translation * toPivot * rotation * scaling * fromPivot;
	}

	/// @brief
	/// @return
	const Matrix4& Node::ComputeCanvasMatrix()
	{
		if (_dirtyFlags & (uint8_t)DirtyFlag::CanvasMatrix)
		{
			if (_parent != nullptr)
			{
				// The parent's transform applies after the node's own one (column vectors)
				_canvasMatrix = _parent->ComputeCanvasMatrix() * _localMatrix;
			}
			else
			{
				_canvasMatrix = _localMatrix;
			}

			_dirtyFlags &= ~((uint8_t)DirtyFlag::CanvasMatrix);
		}

		return _canvasMatrix;
	}

	Matrix4 Node::ComputeWorldMatrix()
	{
		// Scene has no render-mode/scale-mode concept yet (unlike the old UI::Canvas), so canvas
		// space is world space for now; revisit once that concept is designed.
		return ComputeCanvasMatrix();
	}

	/// @brief
	/// @return
	Node* Node::GetParent() const
	{
		return _parent;
	}

	const Vector<Node*>& Node::GetChildren() const
	{
		return _children;
	}

	LayoutParams* Node::GetLayoutParams() const
	{
		return _layoutParams;
	}

	LayoutParams* Node::CreateDefaultLayoutParams() const
	{
		if (_layout != nullptr)
		{
			return _layout->CreateDefaultLayoutParams();
		}
		return DefaultAllocator::GetInstance().New<AnchoredLayoutParams>();
	}

	void Node::SetLayout(Layout* layout)
	{
		if (layout == _layout)
		{
			return;
		}

		if (_layout != nullptr)
		{
			DefaultAllocator::GetInstance().Delete(_layout);
		}

		_layout = layout;
		if (_layout != nullptr)
		{
			_layout->_node = this;
		}

		// Each child's LayoutParams must be of the type the (new) layout expects (e.g. BoxLayoutParams
		// under a BoxLayout, AnchoredLayoutParams under none): convert the ones that aren't, carrying
		// over what the two types share (see AdaptLayoutParams).
		for (Node* child : _children)
		{
			LayoutParams* oldLayoutParams = child->_layoutParams;
			LayoutParams* newLayoutParams = AdaptLayoutParams(oldLayoutParams, CreateDefaultLayoutParams());
			if (newLayoutParams == oldLayoutParams)
			{
				continue;
			}

			child->_layoutParams = newLayoutParams;
			newLayoutParams->_node = child;
			child->MarkMeasureAsDirty();
			child->MarkArrangeAsDirty();
		}

		// Swapping the arrangement algorithm can change both this node's own content-derived size
		// and how its children end up placed, even when that size doesn't change.
		MarkMeasureAsDirty();
		MarkArrangeAsDirty();
	}

	/// @brief Replaces the layout by a new one of the given type, carrying over what the two types share (see
	/// CopyCommonProperties and Layout::CarryOverFrom), e.g. the spacing and padding from a HorizontalLayout to a
	/// VerticalLayout. Without a current layout, attaches a default one of that type.
	void Node::ConvertLayout(const ReflectionDescriptor& layoutDescriptor)
	{
		Layout* layout = layoutDescriptor.CreateInstance<Layout>();
		if (_layout != nullptr)
		{
			CopyCommonProperties(_layout->GetReflectionDescriptorV(), _layout, layout->GetReflectionDescriptorV(), layout);
			layout->CarryOverFrom(*_layout);
		}
		SetLayout(layout);
	}

	Layout* Node::GetLayout() const
	{
		return _layout;
	}

	void Node::SetInteraction(Interaction* interaction)
	{
		if (interaction == _interaction)
		{
			return;
		}

		DefaultAllocator::GetInstance().Delete(_interaction);

		_interaction = interaction;
		if (_interaction != nullptr)
		{
			_interaction->_node = this;
		}

		_propertyChangedEvent.Emit();
	}

	Interaction* Node::GetInteraction() const
	{
		return _interaction;
	}

	bool Node::IsPointerTarget() const
	{
		return _pointerTarget;
	}

	void Node::SetPointerTarget(bool pointerTarget)
	{
		if (_pointerTarget != pointerTarget)
		{
			_pointerTarget = pointerTarget;
			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @param canvasPosition
	/// @return
	Node* Node::HitTest(const Vector2& canvasPosition)
	{
		// The canvas matrix maps this node's own space (origin at its rect's center) to canvas space: back into it, the rect is
		// centered on the origin. A zero scale has no inverse (and nothing drawn to hit).
		bool inside = false;
		if (_size.GetX() > 0.0f && _size.GetY() > 0.0f && _scale.GetX() != 0.0f && _scale.GetY() != 0.0f)
		{
			Vector4 local = Matrix4::Inverse(ComputeCanvasMatrix()) * Vector4(canvasPosition.GetX(), canvasPosition.GetY(), 0.0f, 1.0f);
			inside = std::abs(local.GetX()) <= _size.GetX() * 0.5f && std::abs(local.GetY()) <= _size.GetY() * 0.5f;
		}

		if (inside || ClipsChildren() == false)
		{
			for (uint32_t childIndex = _children.Size(); childIndex > 0; --childIndex)
			{
				Node* hit = _children[childIndex - 1]->HitTest(canvasPosition);
				if (hit != nullptr)
				{
					return hit;
				}
			}
		}

		if (inside && (_pointerTarget || _interaction != nullptr))
		{
			return this;
		}
		return nullptr;
	}

	bool Node::ClipsChildren() const
	{
		return false;
	}

	void Node::SetZOrder(int32_t zOrder)
	{
		_zOrder = zOrder;
	}

	int32_t Node::GetZOrder() const
	{
		return _zOrder;
	}

	Event<>& Node::GetPropertyChangedEvent()
	{
		return _propertyChangedEvent;
	}

	void Node::PushRenderCommand(DrawContext& drawContext)
	{
		DrawSelf(drawContext);
		DrawChildren(drawContext);
	}

	void Node::DrawSelf(DrawContext& /*drawContext*/)
	{
	}

	void Node::DrawChildren(DrawContext& drawContext)
	{
		for (Node* child : _children)
		{
			child->PushRenderCommand(drawContext);
		}
	}

	bool Node::SerializeInDocument(DocumentNode& documentNode)
	{
		if (_prefabInstance != nullptr)
		{
			return SerializePrefabInstance(documentNode);
		}

		if (Serializer::Serialize(*this, documentNode) == false)
		{
			return false;
		}

		if (_layout != nullptr)
		{
			DocumentNode& layoutNode = documentNode.AddChild("Layout");
			layoutNode.AddChild("Type").SetUInt64(_layout->GetReflectionDescriptorV().GetType());
			if (Serializer::Serialize(_layout, layoutNode) == false)
			{
				return false;
			}
		}

		if (_interaction != nullptr)
		{
			DocumentNode& interactionNode = documentNode.AddChild("Interaction");
			interactionNode.AddChild("Type").SetUInt64(_interaction->GetReflectionDescriptorV().GetType());
			if (Serializer::Serialize(_interaction, interactionNode) == false)
			{
				return false;
			}
		}

		DocumentNode& childrenNode = documentNode.AddChild("Children");
		for (Node* child : _children)
		{
			if (child->SerializeAsChildInDocument(childrenNode.AddChild("")) == false)
			{
				return false;
			}
		}

		return true;
	}

	bool Node::SerializeAsChildInDocument(DocumentNode& documentNode)
	{
		documentNode.AddChild("Type").SetUInt64(GetReflectionDescriptorV().GetType());

		if (_layoutParams != nullptr)
		{
			DocumentNode& layoutParamsNode = documentNode.AddChild("LayoutParams");
			layoutParamsNode.AddChild("Type").SetUInt64(_layoutParams->GetReflectionDescriptorV().GetType());
			if (Serializer::Serialize(_layoutParams, layoutParamsNode) == false)
			{
				return false;
			}
		}

		return SerializeInDocument(documentNode);
	}

	namespace
	{
		/// @brief A node of the type entry names. A missing or unknown type (document saved before drawing nodes existed, or by
		/// a module that isn't loaded) degrades to a plain Node rather than failing the whole tree.
		Node* CreateNode(const DocumentNode& entry)
		{
			const DocumentNode* typeNode = entry.GetChild("Type");
			if (typeNode != nullptr)
			{
				const std::map<uint64_t, ReflectionDescriptor*>& descriptors = NodeFactory::GetInstance()->GetAllDescriptors();
				auto                                              it = descriptors.find(typeNode->GetUInt64());
				if (it != descriptors.end())
				{
					return it->second->CreateInstance<Node>();
				}
			}
			return DefaultAllocator::GetInstance().New<Node>();
		}

		/// @brief The entry of a prefab's root in its document (see UIPrefabResource): the single child of the canvas' root node.
		const DocumentNode* FindPrefabRootEntry(const Document& document)
		{
			const DocumentNode* childrenNode = document.GetRootNode().GetChild("Children");
			if (childrenNode == nullptr || childrenNode->GetFirstChild() == nullptr || childrenNode->GetFirstChild()->GetNextSibling() != nullptr)
			{
				return nullptr;
			}
			return childrenNode->GetFirstChild();
		}
	}

	LayoutParams* Node::ReadLayoutParams(const DocumentNode& entry) const
	{
		LayoutParams*       layoutParams = nullptr;
		const DocumentNode* layoutParamsNode = entry.GetChild("LayoutParams");
		if (layoutParamsNode != nullptr)
		{
			const DocumentNode* typeNode = layoutParamsNode->GetChild("Type");
			if (typeNode != nullptr)
			{
				const std::map<uint64_t, ReflectionDescriptor*>& descriptors = LayoutParamsFactory::GetInstance()->GetAllDescriptors();
				auto                                              it = descriptors.find(typeNode->GetUInt64());
				if (it != descriptors.end())
				{
					layoutParams = it->second->CreateInstance<LayoutParams>();
					Serializer::Deserialize(*it->second, layoutParams, *layoutParamsNode);
				}
			}
		}

		// Written under a parent expecting another type (copied from a BoxLayout into an anchored node...): converted.
		LayoutParams* defaultLayoutParams = CreateDefaultLayoutParams();
		return (layoutParams != nullptr) ? AdaptLayoutParams(layoutParams, defaultLayoutParams) : defaultLayoutParams;
	}

	Node* Node::AddChildFromDocument(const DocumentNode& documentNode)
	{
		const DocumentNode* instanceNode = documentNode.GetChild("PrefabInstance");
		if (instanceNode != nullptr)
		{
			return AddPrefabInstanceFromDocument(documentNode, *instanceNode);
		}

		Node* child = CreateNode(documentNode);
		AddChild(child, ReadLayoutParams(documentNode));

		if (child->DeserializeFromDocument(documentNode) == false)
		{
			RemoveChild(child);
			DefaultAllocator::GetInstance().Delete(child);
			return nullptr;
		}

		return child;
	}

	/// @brief An instance's entry: its type and LayoutParams as any node's, then its id in this prefab and a PrefabInstance
	/// node: the prefab (UID) and the overrides.
	Node* Node::AddPrefabInstanceFromDocument(const DocumentNode& entry, const DocumentNode& instanceNode)
	{
		const DocumentNode* prefabNode = instanceNode.GetChild("Prefab");
		UID                 prefab = (prefabNode != nullptr && prefabNode->GetType() == DocumentNode::Type::String) ? UID::FromString(prefabNode->GetString().CStr())
		                                                                                                            : UID::INVALID_UID;
		PrefabInstance*     prefabInstance = DefaultAllocator::GetInstance().New<PrefabInstance>(prefab);

		// The prefab's root, unless the prefab can't be loaded (or is being loaded: it would contain itself)
		Document            prefabDocument;
		const DocumentNode* sourceEntry = nullptr;
		if (prefab.IsValid() == false)
		{
			OUTPUT_ERROR("UI prefab instance without prefab");
		}
		else if (PrefabInstance::IsLoading(prefab))
		{
			OUTPUT_ERROR("UI prefab {} contains itself: its instance is left empty", prefab.ToString());
		}
		else if (PrefabInstance::LoadSource(prefab, prefabDocument) == false || (sourceEntry = FindPrefabRootEntry(prefabDocument)) == nullptr)
		{
			OUTPUT_ERROR("UI prefab {} can't be loaded: its instance is left empty", prefab.ToString());
		}

		Node* child = (sourceEntry != nullptr) ? CreateNode(*sourceEntry) : DefaultAllocator::GetInstance().New<Node>();
		// The placement is the containing prefab's. A new instance (none yet) starts with the one the prefab's root has in its
		// own canvas: its size, anchors...
		const DocumentNode& placementEntry = (entry.GetChild("LayoutParams") == nullptr && sourceEntry != nullptr) ? *sourceEntry : entry;
		AddChild(child, ReadLayoutParams(placementEntry));

		if (sourceEntry != nullptr)
		{
			PrefabInstance::ScopedLoading loading(prefab);
			if (child->DeserializeFromDocument(*sourceEntry) == false)
			{
				RemoveChild(child);
				DefaultAllocator::GetInstance().Delete(child);
				DefaultAllocator::GetInstance().Delete(prefabInstance);
				return nullptr;
			}
			prefabInstance->_source.GetRootNode().Copy(*sourceEntry);
			prefabInstance->_rootLocalId = child->_localId;
			prefabInstance->_missing = false;
		}

		child->_prefabInstance = prefabInstance;
		const DocumentNode* localIdNode = entry.GetChild("_localId");
		child->_localId = (localIdNode != nullptr) ? localIdNode->GetUInt64() : 0;

		Vector<PropertyOverride> overrides;
		if (const DocumentNode* overridesNode = instanceNode.GetChild("Overrides"))
		{
			for (const DocumentNode* overrideNode = overridesNode->GetFirstChild(); overrideNode != nullptr; overrideNode = overrideNode->GetNextSibling())
			{
				Serializer::Deserialize(overrides.EmplaceBack(), *overrideNode);
			}
		}

		if (prefabInstance->_missing)
		{
			prefabInstance->_missingOverrides = std::move(overrides);
			return child;
		}

		for (const PropertyOverride& propertyOverride : overrides)
		{
			Node*         target = child->FindByLocalId(propertyOverride._target);
			PropertyValue value;
			if (target != nullptr && propertyOverride.GetValue(*target, value))
			{
				value.Write(*target, propertyOverride._property);
			}
		}

		return child;
	}

	/// @brief See AddPrefabInstanceFromDocument
	bool Node::SerializePrefabInstance(DocumentNode& documentNode)
	{
		documentNode.AddChild("_localId").SetUInt64(_localId);

		DocumentNode& instanceNode = documentNode.AddChild("PrefabInstance");
		instanceNode.AddChild("Prefab").SetString(_prefabInstance->GetPrefab().ToString());

		Vector<PropertyOverride> overrides;
		_prefabInstance->CollectOverrides(*this, overrides);
		DocumentNode& overridesNode = instanceNode.AddChild("Overrides");
		for (const PropertyOverride& propertyOverride : overrides)
		{
			if (Serializer::Serialize(propertyOverride, overridesNode.AddChild("")) == false)
			{
				return false;
			}
		}

		return true;
	}

	bool Node::DeserializeFromDocument(const DocumentNode& documentNode)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			return false;
		}

		const DocumentNode* layoutNode = documentNode.GetChild("Layout");
		if (layoutNode != nullptr)
		{
			const DocumentNode* typeNode = layoutNode->GetChild("Type");
			if (typeNode != nullptr)
			{
				const std::map<uint64_t, ReflectionDescriptor*>& descriptors = LayoutFactory::GetInstance()->GetAllDescriptors();
				auto                                              it = descriptors.find(typeNode->GetUInt64());
				if (it != descriptors.end())
				{
					Layout* layout = it->second->CreateInstance<Layout>();
					Serializer::Deserialize(*it->second, layout, *layoutNode);
					SetLayout(layout);
				}
			}
		}

		// An unknown type (registered by a module that isn't loaded) leaves the node without interaction.
		const DocumentNode* interactionNode = documentNode.GetChild("Interaction");
		if (interactionNode != nullptr)
		{
			const DocumentNode* typeNode = interactionNode->GetChild("Type");
			if (typeNode != nullptr)
			{
				const std::map<uint64_t, ReflectionDescriptor*>& descriptors = InteractionFactory::GetInstance()->GetAllDescriptors();
				auto                                              it = descriptors.find(typeNode->GetUInt64());
				if (it != descriptors.end())
				{
					Interaction* interaction = it->second->CreateInstance<Interaction>();
					Serializer::Deserialize(*it->second, interaction, *interactionNode);
					SetInteraction(interaction);
				}
			}
		}

		const DocumentNode* childrenNode = documentNode.GetChild("Children");
		if (childrenNode == nullptr)
		{
			return true;
		}

		const DocumentNode* childNode = childrenNode->GetFirstChild();
		while (childNode != nullptr)
		{
			if (AddChildFromDocument(*childNode) == nullptr)
			{
				return false;
			}

			childNode = childNode->GetNextSibling();
		}

		return true;
	}
}
