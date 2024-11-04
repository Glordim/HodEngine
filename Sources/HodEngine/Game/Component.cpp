#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS_NO_PARENT(Component)
		{
			AddPropertyT(this, &Component::_uid, "_uid")->AddTrait<ReflectionTraitHide>();
			AddPropertyT(this, &Component::_localId, "_localId")->AddTrait<ReflectionTraitHide>();
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

		/// @brief 
		/// @param enable 
		void Component::SetEnable(bool enable)
		{
			if (_enable != enable)
			{
				_enable = enable;
				if (_wasEnable != false && _enable == true)
				{
					OnEnable();
					_wasEnable = true;
				}
				else if (_wasEnable != true && _enable == false)
				{
					OnDisable();
					_wasEnable = false;
				}
			}
		}

		/// @brief 
		/// @return 
		bool Component::GetEnable() const
		{
			return _enable;
		}
	}
}
