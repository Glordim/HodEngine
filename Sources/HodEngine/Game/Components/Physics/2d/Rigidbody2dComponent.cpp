#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Physics/2d/Collider2dComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/World.hpp"

#include <HodEngine/Physics/World.hpp>
#include <HodEngine/Physics/Body.hpp>
#include <HodEngine/Physics/Collider.hpp>
#include <HodEngine/Core/Reflection/EnumTrait.hpp>

#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(Rigidbody2dComponent, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Rigidbody2dComponent::_mode, "Mode", &Rigidbody2dComponent::SetMode);
		AddPropertyT(reflectionDescriptor, &Rigidbody2dComponent::_gravityScale, "GravityScale", &Rigidbody2dComponent::SetGravityScale);
	}

	/// @brief 
	Rigidbody2dComponent::~Rigidbody2dComponent()
	{
		GetWorld()->GetPhysicsWorld()->DeleteBody(_body);
	}

	/// @brief 
	void Rigidbody2dComponent::OnConstruct()
	{
		static physics::Body::Type modeToTypeMapping[EnumTrait::GetCount<Mode>()] = {
			physics::Body::Type::Static,
			physics::Body::Type::Kinematic,
			physics::Body::Type::Dynamic,
		};

		Vector2 position = Vector2::Zero;
		float rotation = 0.0f;

		_body = GetWorld()->GetPhysicsWorld()->CreateBody(modeToTypeMapping[std::to_underlying(_mode)], position, rotation);
		_body->SetUserData(this);
		_body->SetMoveEventCallback([this](const Vector2& position, float rotation)
		{
			Entity* entity = GetOwner();
			if (entity != nullptr)
			{
				Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					node2dComponent->SetPosition(position);
					node2dComponent->SetRotation(rotation);
				}
			}
		});
		_body->SetCollisionEnterCallback([this](const physics::Collision& collision)
		{
			CollisionEvent collisionEvent;
			collisionEvent._collider = static_cast<Collider2dComponent*>(collision._colliderA.GetUserData());
			collisionEvent._other = static_cast<Collider2dComponent*>(collision._colliderB.GetUserData());
			collisionEvent._normal = collision._normal;

			_onCollisionEnterEvent.Emit(collisionEvent);
		});
		_body->SetCollisionExitCallback([this](const physics::Collision& collision)
		{
			CollisionEvent collisionEvent;
			collisionEvent._collider = static_cast<Collider2dComponent*>(collision._colliderA.GetUserData());
			collisionEvent._other = static_cast<Collider2dComponent*>(collision._colliderB.GetUserData());
			collisionEvent._normal = collision._normal;

			_onCollisionExitEvent.Emit(collisionEvent);
		});
		_body->SetTriggerEnterCallback([this](const physics::Collider& trigger, const physics::Collider& visitor)
		{
			TriggerEvent triggerEvent;
			triggerEvent._collider = static_cast<Collider2dComponent*>(trigger.GetUserData());
			triggerEvent._other = static_cast<Collider2dComponent*>(visitor.GetUserData());

			_onTriggerEnterEvent.Emit(triggerEvent);
		});
		_body->SetTriggerExitCallback([this](const physics::Collider& trigger, const physics::Collider& visitor)
		{
			TriggerEvent triggerEvent;
			triggerEvent._collider = static_cast<Collider2dComponent*>(trigger.GetUserData());
			triggerEvent._other = static_cast<Collider2dComponent*>(visitor.GetUserData());

			_onTriggerExitEvent.Emit(triggerEvent);
		});

		SetMode(GetMode());
		SetGravityScale(GetGravityScale());
	}

	/// @brief 
	void Rigidbody2dComponent::OnAwake()
	{
		
	}

	/// @brief 
	void Rigidbody2dComponent::OnStart()
	{
		Vector2 position = Vector2::Zero;
		float rotation = 0.0f;
		Vector2 scale = Vector2::One;

		Entity* entity = GetOwner();
		if (entity != nullptr)
		{
			Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				Matrix4 worldMatrix = node2dComponent->GetWorldMatrix();
				position = worldMatrix.GetTranslation();
				rotation = worldMatrix.GetRotation().GetAngleZ();
				//scale = worldMatrix.GetScale();
			}
		}
		_body->SetTransform(position, rotation, scale);

		_body->SetType(_body->GetType());
	}

	/// @brief 
	void Rigidbody2dComponent::OnEnable()
	{
		_body->SetEnabled(true);
	}

	/// @brief 
	void Rigidbody2dComponent::OnFixedUpdate()
	{
		Entity* entity = GetOwner();
		if (entity != nullptr)
		{
			Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				Matrix4 worldMatrix = node2dComponent->GetWorldMatrix();
				Vector2 position = worldMatrix.GetTranslation();
				float rotation = worldMatrix.GetRotation().GetAngleZ();
				Vector2 scale;// = worldMatrix.GetScale();
				_body->SetTransform(position, rotation, scale);
			}
		}

		/*
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
	void Rigidbody2dComponent::OnDisable()
	{
		_body->SetEnabled(false);
	}

	/// @brief 
	/// @return 
	physics::Body* Rigidbody2dComponent::GetInternalBody() const
	{
		return _body;
	}

	/// @brief 
	/// @param collider 
	/// @return 
	Vector2 Rigidbody2dComponent::GetParentOffset(Collider2dComponent* collider) const
	{
		Node2dComponent* colliderNode2dComponent = collider->GetOwner()->GetComponent<Node2dComponent>();
		Node2dComponent* bodyNode2dComponent = GetOwner()->GetComponent<Node2dComponent>();
		if (colliderNode2dComponent == bodyNode2dComponent)
		{
			return Vector2::Zero;
		}

		Matrix4 colliderWorldMatrix = colliderNode2dComponent->GetWorldMatrix();
		Matrix4 bodyWorldMatrix = bodyNode2dComponent->GetWorldMatrix();
		return colliderWorldMatrix.GetTranslation() - bodyWorldMatrix.GetTranslation();
	}

	/// @brief 
	/// @param dynamic 
	void Rigidbody2dComponent::SetMode(Mode mode)
	{
		_mode = mode;
		if (_body != nullptr)
		{
			static physics::Body::Type modeToTypeMapping[EnumTrait::GetCount<Mode>()] = {
				physics::Body::Type::Static,
				physics::Body::Type::Kinematic,
				physics::Body::Type::Dynamic,
			};

			_body->SetType(modeToTypeMapping[std::to_underlying(_mode)]);
		}
	}

	/// @brief 
	/// @return 
	Rigidbody2dComponent::Mode Rigidbody2dComponent::GetMode() const
	{
		return _mode;
	}

	/// @brief 
	/// @param gravityScale 
	void Rigidbody2dComponent::SetGravityScale(float gravityScale)
	{
		_gravityScale = gravityScale;
		if (_body != nullptr)
		{
			_body->SetGravityScale(_gravityScale);
		}
	}

	/// @brief 
	/// @return 
	float Rigidbody2dComponent::GetGravityScale() const
	{
		return _gravityScale;
	}

	/// @brief 
	/// @param velocity 
	void Rigidbody2dComponent::SetVelocity(const Vector2& velocity)
	{
		if (_body != nullptr)
		{
			_body->SetVelocity(velocity);
		}
	}

	/// @brief 
	/// @return 
	Vector2 Rigidbody2dComponent::GetVelocity() const
	{
		if (_body != nullptr)
		{
			return _body->GetVelocity();
		}
		else
		{
			return Vector2::Zero;
		}
	}

	/// @brief 
	/// @return 
	Event<const CollisionEvent&>& Rigidbody2dComponent::GetOnCollisionEnterEvent()
	{
		return _onCollisionEnterEvent;
	}

	/// @brief 
	/// @return 
	Event<const CollisionEvent&>& Rigidbody2dComponent::GetOnCollisionExitEvent()
	{
		return _onCollisionExitEvent;
	}

	/// @brief 
	/// @return 
	Event<const TriggerEvent&>& Rigidbody2dComponent::GetOnTriggerEnterEvent()
	{
		return _onTriggerEnterEvent;
	}

	/// @brief 
	/// @return 
	Event<const TriggerEvent&>& Rigidbody2dComponent::GetOnTriggerExitEvent()
	{
		return _onTriggerExitEvent;
	}
}
