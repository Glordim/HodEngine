#include "HodEngine/Game/Components/NodeComponent.h"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h>

namespace hod
{
	using namespace Reflection::Property;

	namespace game
	{
		//DECLARE_HOD_COMPONENT(NodeComponent, Component)

		DESCRIBE_REFLECTED_CLASS(NodeComponent, Component)
		{
			//AddProperty<Variable>(Variable::Type::UInt64, offsetof(NodeComponent, _parent), "Parent");
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
		const char* NodeComponent::GetType() const
		{
			return "Node";
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
		std::weak_ptr<NodeComponent> NodeComponent::GetChild(uint32_t index)
		{
			return _children[index];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void NodeComponent::SetParent(const std::weak_ptr<NodeComponent>& parent)
		{
			std::shared_ptr<NodeComponent> parentLock = _parent.lock();
			if (parentLock != nullptr)
			{
				std::shared_ptr<NodeComponent> thiz = std::static_pointer_cast<NodeComponent>(shared_from_this());

				auto itEnd = parentLock->_children.end();
				for (auto it = parentLock->_children.begin(); it != itEnd; ++it)
				{
					if (it->lock() == thiz)
					{
						parentLock->_children.erase(it);
						break;
					}
				}
				// todo assert
			}

			parentLock = parent.lock();
			if (parentLock != nullptr)
			{
				parentLock->_children.push_back(std::static_pointer_cast<NodeComponent>(shared_from_this()));
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
		std::weak_ptr<NodeComponent> NodeComponent::GetParent() const
		{
			return _parent;
		}
	}
}
