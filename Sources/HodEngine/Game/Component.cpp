#include "HodEngine/Game/Component.hpp"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS_NO_PARENT(Component)
		{
			ADD_PROPERTY(Component, _uid);
			ADD_PROPERTY(Component, _localId);
		}

		/// @brief 
		Component::Component()
		: _uid(UID::GenerateUID())
		{
		}

		/// @brief 
		/// @param entity 
		void Component::SetEntity(const std::shared_ptr<Entity>& entity)
		{
			_entity = entity;
		}

		/// @brief 
		/// @return 
		std::shared_ptr<Entity> Component::GetEntity() const
		{
			return _entity.lock();
		}
	}
}
