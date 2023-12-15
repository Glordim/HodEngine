#include "HodEngine/Game/Component.hpp"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(Component, Object)
		{
			ADD_PROPERTY(Component, _uid);
		}

		/// @brief 
		Component::Component()
		: _uid(UID::GenerateUID())
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
