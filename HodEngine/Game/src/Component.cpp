#include "Component.h"

#include "Traits/Type.h"

namespace HOD
{
	namespace GAME
	{
		DESCRIBE_REFLECTED_ABSTRACT_CLASS(Component)
		{
			AddTrait<Traits::Type>(Traits::Type::Component);
			//AddTrait<Traits::Hidden>();
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
		void Component::PushToRenderQueue(RENDERER::RenderQueue& renderQueue)
		{

		}
	}
}
