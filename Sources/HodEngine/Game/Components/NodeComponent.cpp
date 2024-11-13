#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Game/Entity.hpp"

namespace hod::game
{
	//DECLARE_HOD_COMPONENT(NodeComponent, Component)

	DESCRIBE_REFLECTED_CLASS(NodeComponent, Component)
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	NodeComponent::~NodeComponent()
	{

	}

	/// @brief 
	void NodeComponent::OnConstruct()
	{
		//SetParent(_parent);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	const Matrix4& NodeComponent::GetLocalMatrix()
	{
		if (_localMatrixDirty == true)
		{
			_localMatrixDirty = false;

			ComputeLocalMatrix(_localMatrix);
		}

		return _localMatrix;
	}

	/// @brief 
	/// @return 
	Matrix4 NodeComponent::GetWorldMatrix()
	{
		std::shared_ptr<Entity> parent = GetEntity()->GetParent().Lock();
		if (parent == nullptr)
		{
			return GetLocalMatrix();
		}

		while (parent != nullptr)
		{
			std::shared_ptr<NodeComponent> parentNodeComponent = parent->GetComponent<NodeComponent>();
			if (parentNodeComponent != nullptr)
			{
				return parentNodeComponent->GetWorldMatrix() * GetLocalMatrix();
			}
			parent = parent->GetParent().Lock();
		}
		return GetLocalMatrix();
	}

	/// @brief 
	void NodeComponent::SetLocalMatrixDirty()
	{
		_localMatrixDirty = true;
	}
}
