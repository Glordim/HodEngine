#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/WeakNode.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(WeakNodeBase)
	{
		HOD_PROPERTY(_localId, &WeakNodeBase::SetLocalId);
	}

	/// @brief
	/// @param nodeDescriptor
	/// @param viewDescriptor
	WeakNodeBase::WeakNodeBase(const ReflectionDescriptor* nodeDescriptor, const ReflectionDescriptor* viewDescriptor)
	: _nodeDescriptor(nodeDescriptor)
	, _viewDescriptor(viewDescriptor)
	{
	}

	/// @brief
	/// @param other
	/// @return
	bool WeakNodeBase::operator==(const WeakNodeBase& other) const
	{
		return _localId == other._localId;
	}

	/// @brief
	/// @return
	uint64_t WeakNodeBase::GetLocalId() const
	{
		return _localId;
	}

	/// @brief
	/// @param localId
	void WeakNodeBase::SetLocalId(uint64_t localId)
	{
		_localId = localId;
		_node = nullptr;
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* WeakNodeBase::GetNodeDescriptor() const
	{
		return _nodeDescriptor;
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* WeakNodeBase::GetViewDescriptor() const
	{
		return _viewDescriptor;
	}

	/// @brief
	/// @param node
	/// @return
	bool WeakNodeBase::Accepts(const Node& node) const
	{
		if (_nodeDescriptor != nullptr && node.GetReflectionDescriptorV().IsCompatible(*_nodeDescriptor) == false)
		{
			return false;
		}
		if (_viewDescriptor != nullptr && (node.GetView() == nullptr || node.GetView()->GetReflectionDescriptorV().IsCompatible(*_viewDescriptor) == false))
		{
			return false;
		}
		return true;
	}

	/// @brief
	/// @return
	Node* WeakNodeBase::GetNode() const
	{
		return _node.Get();
	}

	/// @brief
	/// @param scopeRoot
	/// @return
	bool WeakNodeBase::Resolve(Node& scopeRoot)
	{
		_node = nullptr;
		if (_localId == 0)
		{
			return false;
		}

		Node* node = scopeRoot.FindByLocalId(_localId);
		if (node == nullptr)
		{
			OUTPUT_WARNING("UI view: the node {} referenced isn't in the prefab anymore", _localId);
			return false;
		}
		if (Accepts(*node) == false)
		{
			OUTPUT_WARNING("UI view: the node '{}' referenced isn't a {}", node->GetName(),
			               (_viewDescriptor != nullptr) ? _viewDescriptor->GetDisplayName() : _nodeDescriptor->GetDisplayName());
			return false;
		}

		_node = node;
		return true;
	}

	/// @brief
	void WeakNodeBase::Reset()
	{
		_node = nullptr;
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void WeakNodeBase::Remap(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		if (_localId == 0)
		{
			return;
		}

		auto it = oldToNew.find(_localId);
		if (it != oldToNew.end())
		{
			SetLocalId(it->second);
		}
		else if (keepOthers == false)
		{
			SetLocalId(0);
		}
	}
}
