#include "RendererComponent.h"

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererComponent::RendererComponent(Actor* actor) : Component(actor)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* RendererComponent::GetType() const
		{
			return "Renderer";
		}
	}
}
