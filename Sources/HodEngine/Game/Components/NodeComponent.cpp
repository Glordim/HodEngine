#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>

namespace hod
{
	namespace game
	{
		//DECLARE_HOD_COMPONENT(NodeComponent, Component)

		DESCRIBE_REFLECTED_CLASS(NodeComponent, Component)
		{
			AddPropertyT(this, &NodeComponent::_parent, "Parent", &NodeComponent::SetParent)->AddTrait<ReflectionTraitHide>();
			AddPropertyT(this, &NodeComponent::_children, "Children")->AddTrait<ReflectionTraitHide>();
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

		/// @brief 
		/// @return 
		uint32_t NodeComponent::GetSiblingIndex() const
		{
			std::shared_ptr<NodeComponent> parentLock = _parent.Lock();
			if (parentLock != nullptr)
			{
				for (uint32_t index = 0; index < parentLock->GetChildCount(); ++index)
				{
					if (parentLock->GetChild(index).Lock().get() == this)
					{
						return index;
					}
				}
			}
			return 0;
		}

		/// @brief 
		/// @param index 
		void NodeComponent::SetSiblingIndex(uint32_t index)
		{
			std::shared_ptr<NodeComponent> parentLock = _parent.Lock();
			if (parentLock != nullptr)
			{
				uint32_t initialIndex = GetSiblingIndex();
				for (uint32_t i = initialIndex; i > index; --i)
				{
					parentLock->_children[i] = parentLock->_children[i - 1];
				}
				parentLock->_children[index] = std::static_pointer_cast<NodeComponent>(shared_from_this());
			}
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
