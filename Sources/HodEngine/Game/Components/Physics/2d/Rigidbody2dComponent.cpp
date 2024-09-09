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
}
