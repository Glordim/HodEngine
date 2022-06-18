#include "Component.h"

#include "Traits/Type.h"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_ABSTRACT_CLASS(Component)
		{
			AddTrait<Traits::Type>(Traits::Type::Component);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Component::Component(Actor* actor)
			: _actor(actor)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Component::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{

		}
	}
}
