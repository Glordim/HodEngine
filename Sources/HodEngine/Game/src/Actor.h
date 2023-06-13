#pragma once

#include <map>
#include <typeinfo>
#include <string>
#include <string_view>

#include "Component.h"
#include "Components/SceneComponent.h"

namespace hod
{
	namespace physics
	{
		class Actor;
	}

	namespace game
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Actor
		{
			friend class Scene;

		public:

											Actor() = delete;
											Actor(const std::string& name, Scene* scene);
											Actor(const Actor&) = delete;
											Actor(Actor&&) = delete;
			virtual							~Actor();

			void							operator=(const Actor&) = delete;
			void							operator=(Actor&&) = delete;

		public:

			virtual void					Start();
			virtual void					Update(float dt);
			virtual void					DrawImGui();

			template<typename T>
			T*								AddComponent();
			template<typename T>
			T*								GetComponent() const;
			template<typename T>
			std::vector<T*>					GetAllComponent() const;
			std::vector<Component*>			GetAllComponent() const;

			void							SetName(const std::string_view& name);
			const std::string&				GetName() const;
			Scene*							GetScene() const;

			physics::Actor*					GetPhysicActor() const;
			void							SetPhysicActor(physics::Actor* physicActor);

			void							PushToRenderQueue(renderer::RenderQueue& renderQueue, bool recursive);

		protected:

			std::string						_name;
			physics::Actor*					_physicActor = nullptr;
			Scene*							_scene = nullptr;

		private:

			std::map<size_t, Component*>	_componentMapping;
		};
	}
}

#include "Actor.inl"
