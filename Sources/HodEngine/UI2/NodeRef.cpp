#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/NodeRef.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(NodeRefBase, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &NodeRefBase::_localId, "LocalId", &NodeRefBase::SetLocalId);
	}

	/// @brief
	/// @param nodeDescriptor
	/// @param viewDescriptor
	NodeRefBase::NodeRefBase(const ReflectionDescriptor* nodeDescriptor, const ReflectionDescriptor* viewDescriptor)
	: _nodeDescriptor(nodeDescriptor)
	, _viewDescriptor(viewDescriptor)
	{
	}

	/// @brief
	/// @param other
	/// @return
	bool NodeRefBase::operator==(const NodeRefBase& other) const
	{
		return _localId == other._localId;
	}

	/// @brief
	/// @return
	uint64_t NodeRefBase::GetLocalId() const
	{
		return _localId;
	}

	/// @brief
	/// @param localId
	void NodeRefBase::SetLocalId(uint64_t localId)
	{
		_localId = localId;
		_node = nullptr;
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* NodeRefBase::GetNodeDescriptor() const
	{
		return _nodeDescriptor;
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* NodeRefBase::GetViewDescriptor() const
	{
		return _viewDescriptor;
	}

	/// @brief
	/// @param node
	/// @return
	bool NodeRefBase::Accepts(const Node& node) const
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
	Node* NodeRefBase::GetNode() const
	{
		return _node;
	}

	/// @brief
	/// @param scopeRoot
	/// @return
	bool NodeRefBase::Resolve(Node& scopeRoot)
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
	void NodeRefBase::Reset()
	{
		_node = nullptr;
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void NodeRefBase::Remap(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
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
