#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>

namespace hod
{
	namespace game
	{
		//DECLARE_HOD_COMPONENT(NodeComponent, Component)

		DESCRIBE_REFLECTED_CLASS(NodeComponent, Component)
		{
			ADD_PROPERTY_WITH_SET_METHOD(NodeComponent, _parent, SetParent);
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
			SetParent(_parent);
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
			if (_parent.Lock() == nullptr)
			{
				return GetLocalMatrix();
			}
			
			return _parent.Lock()->GetWorldMatrix() * GetLocalMatrix();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t NodeComponent::GetChildCount() const
		{
			return static_cast<uint32_t>(_children.size());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const WeakComponent<NodeComponent>& NodeComponent::GetChild(uint32_t index)
		{
			return _children[index];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void NodeComponent::SetParent(const WeakComponent<NodeComponent>& parent)
		{
			std::shared_ptr<NodeComponent> parentLock = _parent.Lock();
			if (parentLock != nullptr)
			{
				std::shared_ptr<NodeComponent> thiz = std::static_pointer_cast<NodeComponent>(shared_from_this());

				auto itEnd = parentLock->_children.end();
				for (auto it = parentLock->_children.begin(); it != itEnd; ++it)
				{
					if (it->Lock() == thiz)
					{
						parentLock->_children.erase(it);
						break;
					}
				}
				// todo assert
			}

			parentLock = parent.Lock();
			if (parentLock != nullptr)
			{
				parentLock->_children.emplace_back(std::static_pointer_cast<NodeComponent>(shared_from_this()));
			}
			_parent = parent;
		}

		/// @brief 
		void NodeComponent::SetLocalMatrixDirty()
		{
			_localMatrixDirty = true;
		}

		/// @brief 
		/// @return 
		const WeakComponent<NodeComponent>& NodeComponent::GetParent() const
		{
			return _parent;
		}
	}
}
