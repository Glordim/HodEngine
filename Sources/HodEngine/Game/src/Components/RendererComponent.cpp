#include "HodEngine/Game/src/Components/RendererComponent.h"

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RendererComponent::RendererComponent(const std::weak_ptr<Entity>& entity) : Component(entity)
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
