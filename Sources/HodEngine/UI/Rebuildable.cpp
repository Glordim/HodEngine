#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Rebuildable.hpp"

#include "HodEngine/UI/Canvas.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Rebuildable, reflectionDescriptor)
	{
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
