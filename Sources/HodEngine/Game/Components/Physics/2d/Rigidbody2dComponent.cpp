#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Physics/2d/Rigidbody2dComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"

#include <HodEngine/Physics/Physics.hpp>
#include <HodEngine/Physics/Body.hpp>
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
		physics::Physics::GetInstance()->DeleteBody(_body);
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

		_body = physics::Physics::GetInstance()->CreateBody(modeToTypeMapping[std::to_underlying(_mode)], position, rotation);
		_body->SetMoveEventCallback([this](const Vector2& position, float rotation)
		{
			std::shared_ptr<Entity> entity = GetOwner();
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
		_body->SetTriggerEnterCallback([this](const physics::Collider& trigger, const physics::Collider& visitor)
		{
			_onTriggerEnterEvent.Emit(trigger, visitor);
		});
		_body->SetTriggerExitCallback([this](const physics::Collider& trigger, const physics::Collider& visitor)
		{
			_onTriggerExitEvent.Emit(trigger, visitor);
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

		std::shared_ptr<Entity> entity = GetOwner();
		if (entity != nullptr)
		{
			std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>();
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
	void Rigidbody2dComponent::OnUpdate(float deltaTime)
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

	/// @brief 
	/// @return 
	Event<const physics::Collider&, const physics::Collider&>& Rigidbody2dComponent::GetOnTriggerEnterEvent()
	{
		return _onTriggerEnterEvent;
	}

	/// @brief 
	/// @return 
	Event<const physics::Collider&, const physics::Collider&>& Rigidbody2dComponent::GetOnTriggerExitEvent()
	{
		return _onTriggerExitEvent;
	}
}
