#include "HodEngine/Game/src/Component.h"

#include "HodEngine/Game/src/Traits/Type.h"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(Component, Object)
		{
			AddTrait<Traits::Type>(Traits::Type::Component);
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
