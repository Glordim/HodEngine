#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/Layout.hpp"
#include "HodEngine/UI2/Node.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(Layout, reflectionDescriptor)
	{
		(void)reflectionDescriptor; // abstract interface, no own serializable fields
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
