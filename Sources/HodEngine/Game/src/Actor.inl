
namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<typename T>
		T* Actor::AddComponent()
		{
			T* component = GetComponent<T>();

			if (component == nullptr)
			{
				component = new T(this);
				_componentMapping.emplace(typeid(T).hash_code(), component);
			}

			return component;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<typename T>
		T* Actor::GetComponent() const
		{
			auto it = _componentMapping.find(typeid(T).hash_code());

			if (it == _componentMapping.end())
			{
				return nullptr;
			}
			else
			{
				return (T*)it->second;
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<typename T>
		std::vector<T*> Actor::GetAllComponent() const
		{
			std::vector<T*> ret;

			auto it = _componentMapping.find(typeid(T).hash_code());

			if (it != _componentMapping.end())
			{
				ret.push_back((T*)it->second);
			}

			SceneComponent* sceneComponent = GetComponent<SceneComponent>();
			if (sceneComponent != nullptr)
			{
				size_t childCount = sceneComponent->GetChildCount();
				for (int i = 0; i < childCount; ++i)
				{
					std::vector<T*> componentInChild = sceneComponent->GetChild(i)->GetActor()->GetAllComponent<T>();

					ret.insert(ret.end(), componentInChild.begin(), componentInChild.end());
				}
			}

			return ret;
		}
	}
}
