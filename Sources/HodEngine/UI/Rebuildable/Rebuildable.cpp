#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Rebuildable/Rebuildable.hpp"

#include "HodEngine/UI/Canvas.hpp"

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(Rebuildable, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	void Rebuildable::SetDirty()
	{
		Canvas* canvas = GetOwner()->GetComponentInParent<Canvas>();
		if (canvas != nullptr)
		{
			canvas->MarkForRebuild(this);
		}
	}
}
