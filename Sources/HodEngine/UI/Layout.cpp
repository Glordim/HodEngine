#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout.hpp"
#include "HodEngine/UI/Node.hpp"

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Layout, reflectionDescriptor)
	{
		(void)reflectionDescriptor; // abstract interface, no own serializable fields
	}

	void Layout::CarryOverFrom(const Layout& previous)
	{
		(void)previous;
	}

	void Layout::MarkOwnerMeasureAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkMeasureAsDirty();
		}
	}

	void Layout::MarkOwnerArrangeAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkArrangeAsDirty();
		}
	}
}
