#include "Actor.h"

#include <HodEngine/Physics/Actor.h>

namespace hod
{
	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor::Actor(const std::string& name, Scene* scene)
			: _name(name)
			, _scene(scene)
			, _physicActor(nullptr)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Actor::~Actor()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::Start()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::Update(float dt)
		{
			if (_physicActor != nullptr)
			{
				if (_physicActor->GetType() != physics::Actor::TYPE::Static)
				{
					GetComponent<SceneComponent>()->SetPosition(_physicActor->GetPosition());
					GetComponent<SceneComponent>()->SetRotation(_physicActor->GetRotation());
				}
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::DrawImGui()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Scene* Actor::GetScene() const
		{
			return _scene;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::string& Actor::GetName() const
		{
			return _name;
		}

		/// @brief 
		/// @param name 
		void Actor::SetName(const std::string_view& name)
		{
			_name = name;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		physics::Actor* Actor::GetPhysicActor() const
		{
			return _physicActor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::SetPhysicActor(physics::Actor* physicActor)
		{
			_physicActor = physicActor;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		std::vector<Component*> Actor::GetAllComponent() const
		{
			std::vector<Component*> vComponents;

			auto it = _componentMapping.begin();
			auto itEnd = _componentMapping.end();

			while (it != itEnd)
			{
				vComponents.push_back(it->second);

				++it;
			}

			return vComponents;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Actor::PushToRenderQueue(renderer::RenderQueue& renderQueue, bool recursive)
		{
			auto it = _componentMapping.begin();
			auto itEnd = _componentMapping.end();

			SceneComponent* sceneComponent = nullptr;

			while (it != itEnd)
			{
				it->second->PushToRenderQueue(renderQueue);
				
				if (it->first == typeid(SceneComponent).hash_code())
				{
					sceneComponent = static_cast<SceneComponent*>(it->second);
				}

				++it;
			}

			if (recursive == true)
			{
				if (sceneComponent != nullptr)
				{
					uint32_t childCount = sceneComponent->GetChildCount();
					for (uint32_t childIndex = 0; childIndex < childCount; ++childIndex)
					{
						sceneComponent->GetChild(childIndex)->GetActor()->PushToRenderQueue(renderQueue, recursive);
					}
				}
			}
		}
	}
}
