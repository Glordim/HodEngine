#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/ReflectionHelper.hpp>
#include <memory>
#include <functional>

#include "HodEngine/Core/Type.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Object.hpp"

namespace hod
{
	namespace renderer
	{
		class RenderQueue;
	}

	namespace game
	{
		class Entity;
		class World;

		///@brief 
		class Component : public Object, public std::enable_shared_from_this<Component>
		{
			REFLECTED_CLASS(Component, Object)

			friend class World;

		public:

			void							SetEntity(const std::weak_ptr<Entity>& entity);
			const std::weak_ptr<Entity>&	GetEntity() const;
			virtual const char*				GetType() const = 0;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue);

			const UID&			GetUid() const { return _uid; }

		protected:

								Component();
								Component(const Component&) = delete;
								Component(Component&&) = delete;
			virtual				~Component() = default;

			void				operator=(const Component&) = delete;
			void				operator=(Component&&) = delete;

			virtual void		OnConstruct() {};

		private:

			void				Construct() { OnConstruct(); };

		private:

			UID						_uid;
			std::weak_ptr<Entity>	_entity;
		};
	}
}

#include "Component.inl"
