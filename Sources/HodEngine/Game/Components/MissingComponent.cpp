#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/MissingComponent.hpp"

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(MissingComponent, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param componentNode
	void MissingComponent::SetData(const DocumentNode& componentNode)
	{
		_data.GetRootNode().Copy(componentNode);
	}

	/// @brief
	/// @return
	const DocumentNode& MissingComponent::GetData() const
	{
		return _data.GetRootNode();
	}

	/// @brief
	/// @return
	uint64_t MissingComponent::GetMissingType() const
	{
		const DocumentNode* metaTypeNode = _data.GetRootNode().GetChild("MetaType");
		return metaTypeNode != nullptr ? metaTypeNode->GetUInt64() : 0;
	}
}
