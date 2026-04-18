#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include "HodEngine/Game/Entity.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(NodeComponent, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	NodeComponent::~NodeComponent()
	{

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
		Entity* parent = GetOwner()->GetParent().Lock();
		if (parent == nullptr)
		{
			return GetLocalMatrix();
		}

		while (parent != nullptr)
		{
			NodeComponent* parentNodeComponent = parent->GetComponent<NodeComponent>();
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
