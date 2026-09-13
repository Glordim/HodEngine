#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(LayoutParams, reflectionDescriptor)
	{
		(void)reflectionDescriptor; // abstract interface, no own serializable fields
	}

	void LayoutParams::MarkOwnerSizeAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkSizeAsDirty();
		}
	}

	void LayoutParams::MarkOwnerLocalMatrixAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkLocalMatrixAsDirty();
		}
	}
}
