#include "HodEngine/Game/Components/RendererComponent.h"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(RendererComponent, Component)
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