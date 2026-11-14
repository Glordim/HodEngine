#include "HodEngine/PhysicsComponents/Pch.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Collider2dComponent.hpp"
#include "HodEngine/PhysicsComponents/Components/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/World.hpp"

#include <HodEngine/Core/Reflection/EnumTrait.hpp>
#include <HodEngine/Math/Math.hpp>
#include <HodEngine/Physics/Body.hpp>
#include <HodEngine/Physics/Collider.hpp>
#include <HodEngine/Physics/World.hpp>
#include <HodEngine/PhysicsComponents/PhysicsWorldSystem.hpp>

#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/Traits/ReflectionTraitRequireComponent.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCategory.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDescription.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp"
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline physics
{
	DESCRIBE_REFLECTED_ENUM(Rigidbody2dComponent::Mode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Rigidbody2dComponent::Mode::Static, "Static");
		reflectionDescriptor.AddEnumValue(Rigidbody2dComponent::Mode::Kinematic, "Kinematic");
		reflectionDescriptor.AddEnumValue(Rigidbody2dComponent::Mode::Dynamic, "Dynamic");
	}

	DESCRIBE_REFLECTED_CLASS(Rigidbody2dComponent, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_WEIGHT);
		reflectionDescriptor.AddTrait<ReflectionTraitCategory>("Physics");
		reflectionDescriptor.AddTrait<ReflectionTraitDescription>("Moves the entity and its colliders with the 2D physics: static, kinematic or dynamic");
		reflectionDescriptor.AddTrait<ReflectionTraitRequireComponent>(&Node2dComponent::GetReflectionDescriptor);
		HOD_PROPERTY(_mode, &Rigidbody2dComponent::SetMode);
		HOD_PROPERTY(_gravityScale, &Rigidbody2dComponent::SetGravityScale);
		HOD_PROPERTY(_fixedRotation, &Rigidbody2dComponent::SetFixedRotation);
		HOD_PROPERTY(_continuousCollision, &Rigidbody2dComponent::SetContinuousCollision);
		HOD_PROPERTY(_freezePositionX, &Rigidbody2dComponent::SetFreezePositionX);
		HOD_PROPERTY(_freezePositionY, &Rigidbody2dComponent::SetFreezePositionY);
	}

	/// @brief
	Rigidbody2dComponent::~Rigidbody2dComponent() {}

	/// @brief
	void Rigidbody2dComponent::OnDestruct()
	{
		PhysicsWorldSystem::GetPhysicsWorld(*GetWorld())->DeleteBody(_body);
		_body = nullptr;
	}

	/// @brief
	void Rigidbody2dComponent::OnConstruct()
	{
		static Body::Type modeToTypeMapping[static_cast<uint32_t>(Mode::Count)] = {
			Body::Type::Static,
			Body::Type::Kinematic,
			Body::Type::Dynamic,
		};

		Vector2 position = Vector2::Zero;
		float   rotation = 0.0f;

		_body = PhysicsWorldSystem::GetPhysicsWorld(*GetWorld())->CreateBody(modeToTypeMapping[std::to_underlying(_mode)], position, rotation);
		_body->SetUserData(this);
		_body->SetMoveEventCallback(
			[this](const Vector2& position, float rotation)
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
		_body->SetCollisionEnterCallback(
			[this](const Collision& collision)
			{
				CollisionEvent collisionEvent;
				collisionEvent._collider = static_cast<Collider2dComponent*>(collision._colliderA.GetUserData());
				collisionEvent._other = static_cast<Collider2dComponent*>(collision._colliderB.GetUserData());
				collisionEvent._normal = collision._normal;

				_onCollisionEnterEvent.Emit(collisionEvent);
			});
		_body->SetCollisionExitCallback(
			[this](const Collision& collision)
			{
				CollisionEvent collisionEvent;
				collisionEvent._collider = static_cast<Collider2dComponent*>(collision._colliderA.GetUserData());
				collisionEvent._other = static_cast<Collider2dComponent*>(collision._colliderB.GetUserData());
				collisionEvent._normal = collision._normal;

				_onCollisionExitEvent.Emit(collisionEvent);
			});
		_body->SetTriggerEnterCallback(
			[this](const Collider& trigger, const Collider& visitor)
			{
				TriggerEvent triggerEvent;
				triggerEvent._collider = static_cast<Collider2dComponent*>(trigger.GetUserData());
				triggerEvent._other = static_cast<Collider2dComponent*>(visitor.GetUserData());

				_onTriggerEnterEvent.Emit(triggerEvent);
			});
		_body->SetTriggerExitCallback(
			[this](const Collider& trigger, const Collider& visitor)
			{
				TriggerEvent triggerEvent;
				triggerEvent._collider = static_cast<Collider2dComponent*>(trigger.GetUserData());
				triggerEvent._other = static_cast<Collider2dComponent*>(visitor.GetUserData());

				_onTriggerExitEvent.Emit(triggerEvent);
			});

		SetMode(GetMode());
		SetGravityScale(GetGravityScale());
		SetFixedRotation(GetFixedRotation());
		SetContinuousCollision(GetContinuousCollision());
		_body->SetFreezePosition(_freezePositionX, _freezePositionY);
	}

	/// @brief
	void Rigidbody2dComponent::OnAwake() {}

	/// @brief
	void Rigidbody2dComponent::OnStart()
	{
		TeleportToNode();

		_body->SetType(_body->GetType());
	}

	/// @brief
	void Rigidbody2dComponent::OnEnable()
	{
		// Placed already: the shapes added before the start (in edit mode, the components building theirs in OnEnable) are where the node is
		TeleportToNode();
		_body->SetEnabled(true);
	}

	/// @brief
	bool Rigidbody2dComponent::GetNodeTransform(Vector2& position, float& rotation) const
	{
		Entity* entity = GetOwner();
		if (entity != nullptr)
		{
			Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
			if (node2dComponent != nullptr)
			{
				Matrix4 worldMatrix = node2dComponent->GetWorldMatrix();
				position = worldMatrix.GetTranslation();
				rotation = RadianToDegree(worldMatrix.GetRotation().GetAngleZ()); // the body's in degrees, as the node's
				return true;
			}
		}
		return false;
	}

	/// @brief
	void Rigidbody2dComponent::TeleportToNode()
	{
		Vector2 position;
		float   rotation = 0.0f;
		if (_body != nullptr && GetNodeTransform(position, rotation))
		{
			_body->SetTransform(position, rotation, Vector2::One);
			if (_mode == Mode::Kinematic)
			{
				// Its velocity was only to reach its node (see OnFixedUpdate): there already, it stops
				_body->MoveTo(position, rotation, GetWorld()->GetFixedTimestep());
			}
		}
	}

	/// @brief
	void Rigidbody2dComponent::OnFixedUpdate()
	{
		// A kinematic body goes where its node is over the step (a velocity, not a teleport): what it pushes is pushed, not crossed
		Vector2 position;
		float   rotation = 0.0f;
		if (_mode == Mode::Kinematic && _body != nullptr && GetNodeTransform(position, rotation))
		{
			_body->MoveTo(position, rotation, GetWorld()->GetFixedTimestep());
		}
		else
		{
			TeleportToNode();
		}

		/*
		// todo check if have slot connected ?
		Vector<Collision> collisions;
		_body->GetCollisions(collisions);
		for (const Collision& collision : collisions)
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
	Body* Rigidbody2dComponent::GetInternalBody() const
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
			static Body::Type modeToTypeMapping[static_cast<uint32_t>(Mode::Count)] = {
				Body::Type::Static,
				Body::Type::Kinematic,
				Body::Type::Dynamic,
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
	/// @param fixedRotation
	void Rigidbody2dComponent::SetFixedRotation(bool fixedRotation)
	{
		_fixedRotation = fixedRotation;
		if (_body != nullptr)
		{
			_body->SetFixedRotation(_fixedRotation);
		}
	}

	/// @brief
	/// @return
	bool Rigidbody2dComponent::GetFixedRotation() const
	{
		return _fixedRotation;
	}

	/// @brief
	/// @param continuousCollision
	void Rigidbody2dComponent::SetContinuousCollision(bool continuousCollision)
	{
		_continuousCollision = continuousCollision;
		if (_body != nullptr)
		{
			_body->SetContinuousCollision(_continuousCollision);
		}
	}

	/// @brief
	/// @return
	bool Rigidbody2dComponent::GetContinuousCollision() const
	{
		return _continuousCollision;
	}

	/// @brief
	/// @param freeze
	void Rigidbody2dComponent::SetFreezePositionX(bool freeze)
	{
		_freezePositionX = freeze;
		if (_body != nullptr)
		{
			_body->SetFreezePosition(_freezePositionX, _freezePositionY);
		}
	}

	/// @brief
	/// @return
	bool Rigidbody2dComponent::GetFreezePositionX() const
	{
		return _freezePositionX;
	}

	/// @brief
	/// @param freeze
	void Rigidbody2dComponent::SetFreezePositionY(bool freeze)
	{
		_freezePositionY = freeze;
		if (_body != nullptr)
		{
			_body->SetFreezePosition(_freezePositionX, _freezePositionY);
		}
	}

	/// @brief
	/// @return
	bool Rigidbody2dComponent::GetFreezePositionY() const
	{
		return _freezePositionY;
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
	/// @param force
	void Rigidbody2dComponent::AddForce(const Vector2& force)
	{
		if (_body != nullptr)
		{
			_body->AddForce(force);
		}
	}

	/// @brief
	/// @param impulse
	void Rigidbody2dComponent::AddImpulse(const Vector2& impulse)
	{
		if (_body != nullptr)
		{
			_body->AddImpulse(impulse);
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
