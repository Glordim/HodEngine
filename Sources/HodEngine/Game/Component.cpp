#include "HodEngine/Game/Component.h"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(Component, Object)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Component::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{

		}

		/// @brief 
		/// @param entity 
		void Component::SetEntity(const std::weak_ptr<Entity>& entity)
		{
			_entity = entity;
		}

		/// @brief 
		/// @return 
		const std::weak_ptr<Entity>& Component::GetEntity() const
		{
			return _entity;
		}
	}
}
