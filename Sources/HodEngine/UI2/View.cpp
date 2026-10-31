#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/View.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/NodeRef.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(View, reflectionDescriptor)
	{
		(void)reflectionDescriptor; // base interface, no own serializable fields
	}

	/// @brief
	/// @return
	Node* View::GetNode() const
	{
		return _node;
	}

	/// @brief
	void View::Bind()
	{
		if (_bound || _node == nullptr)
		{
			return;
		}

		Vector<NodeRefBase*> references;
		CollectReferences(references);
		Node* scopeRoot = _node->GetScopeRoot();
		for (NodeRefBase* reference : references)
		{
			reference->Resolve(*scopeRoot);
		}

		_bound = true;
		OnBind();
	}

	/// @brief
	void View::Unbind()
	{
		if (_bound == false)
		{
			return;
		}

		OnUnbind();
		_bound = false;

		Vector<NodeRefBase*> references;
		CollectReferences(references);
		for (NodeRefBase* reference : references)
		{
			reference->Reset();
		}
	}

	/// @brief
	/// @return
	bool View::IsBound() const
	{
		return _bound;
	}

	/// @brief
	/// @param outReferences
	void View::CollectReferences(Vector<NodeRefBase*>& outReferences)
	{
		for (ReflectionDescriptor* descriptor = &GetReflectionDescriptorV(); descriptor != nullptr; descriptor = descriptor->GetParent())
		{
			descriptor->CollectObjectProperties(outReferences, this);
		}
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void View::RemapReferences(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		Vector<NodeRefBase*> references;
		CollectReferences(references);
		for (NodeRefBase* reference : references)
		{
			reference->Remap(oldToNew, keepOthers);
		}
	}

	/// @brief
	void View::OnBind()
	{
	}

	/// @brief
	void View::OnUnbind()
	{
	}
}
