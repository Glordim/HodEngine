#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/Body.hpp>

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(Rigidbody2dComponent, Component)
	{
		AddPropertyT(this, &Rigidbody2dComponent::_dynamic, "Dynamic", &Rigidbody2dComponent::SetDynamic);
	}

	/// @brief 
	Rigidbody2dComponent::~Rigidbody2dComponent()
	{
		physics::Physics::GetInstance()->DeleteBody(_body);
	}

	/// @brief 
	void Rigidbody2dComponent::OnAwake()
	{
		_body = physics::Physics::GetInstance()->CreateBody();
		_body->SetMoveEventCallback([this](const Vector2& position, float rotation)
		{
			std::shared_ptr<Entity> entity = GetEntity();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					node2dComponent->SetPosition(position);
					node2dComponent->SetRotation(rotation);
				}
			}
		});
		_body->SetCollisionEnterCallback([this](const physics::Collision& collision)
		{
			_onCollisionEnterEvent.Emit(collision);
		});
		_body->SetCollisionExitCallback([this](const physics::Collision& collision)
		{
			_onCollisionExitEvent.Emit(collision);
		});

		std::shared_ptr<Entity> entity = GetEntity();
		if (entity != nullptr)
		{
			std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				_body->SetTransform(node2dComponent->GetPosition(), node2dComponent->GetRotation(), node2dComponent->GetScale());
			}
		}

		if (_dynamic)
		{
			_body->SetType(physics::Body::Type::Dynamic);
		}
	}

	/// @brief 
	void Rigidbody2dComponent::OnStart()
	{
		_body->SetType(_body->GetType());
	}

	/// @brief 
	void Rigidbody2dComponent::OnUpdate()
	{
		/*
		std::shared_ptr<Entity> entity = GetEntity();
		if (entity != nullptr)
		{
			std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				node2dComponent->SetPosition(_body->GetPosition());
				node2dComponent->SetRotation(_body->GetRotation());
			}
		}

		// todo check if have slot connected ?
		std::vector<physics::Collision> collisions;
		_body->GetCollisions(collisions);
		for (const physics::Collision& collision : collisions)
		{
			_onCollisionEnterEvent.Emit(collision);
		}
		*/
	}

	/// @brief 
	/// @return 
	physics::Body* Rigidbody2dComponent::GetInternalBody() const
	{
		return _body;
	}

	/// @brief 
	/// @param dynamic 
	void Rigidbody2dComponent::SetDynamic(bool dynamic)
	{
		_dynamic = dynamic;
		if (_body != nullptr)
		{
			if (_dynamic)
			{
				_body->SetType(physics::Body::Type::Dynamic);
			}
			else
			{
				_body->SetType(physics::Body::Type::Static);
			}
		}
	}

	/// @brief 
	/// @return 
	bool Rigidbody2dComponent::IsDynamic() const
	{
		return _dynamic;
	}

	/// @brief 
	/// @return 
	Event<const physics::Collision&>& Rigidbody2dComponent::GetOnCollisionEnterEvent()
	{
		return _onCollisionEnterEvent;
	}

	/// @brief 
	/// @return 
	Event<const physics::Collision&>& Rigidbody2dComponent::GetOnCollisionExitEvent()
	{
		return _onCollisionExitEvent;
	}
}