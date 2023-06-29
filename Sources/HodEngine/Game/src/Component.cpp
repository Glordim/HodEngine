#include "HodEngine/Game/src/Component.h"

#include "HodEngine/Game/src/Traits/Type.h"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_DERIVED_ABSTRACT_CLASS(Component, Object)
		{
			AddTrait<Traits::Type>(Traits::Type::Component);
		}

		/// @brief 
		/// @param entity 
		Component::Component(const std::weak_ptr<Entity>& entity)
		: _entity(entity)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Component::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{

		}

		/// @brief 
		/// @return 
		const std::weak_ptr<Entity>& Component::GetEntity() const
		{
			return _entity;
		}
	}
}
