#include "NodeComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h>

namespace hod
{
	using namespace core::Reflection::Property;

	namespace game
	{
		DESCRIBE_REFLECTED_DERIVED_CLASS(NodeComponent, Component)
		{
			//AddProperty<Variable>(Variable::Type::Float32, offsetof(NodeComponent, _rotation), "Rotation"); // parent
		}

		/// @brief 
		/// @param actor 
		NodeComponent::NodeComponent(Actor* actor) : Component(actor)
		{
			_parent = nullptr;
			_localMatrixDirty = true;
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
		const glm::mat4& NodeComponent::GetLocalMatrix()
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
			return static_cast<uint32_t>(_childs.size());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		NodeComponent* NodeComponent::GetChild(uint32_t index)
		{
			return _childs[index];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void NodeComponent::SetParent(NodeComponent* parent)
		{
			// TODO retirer du vector childs du precedent parent

			_parent = parent;
			_parent->_childs.push_back(this);
		}

		/// @brief 
		void NodeComponent::SetLocalMatrixDirty()
		{
			_localMatrixDirty = true;
		}
	}
}
