#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.h"

namespace hod
{
	///@brief 
	ReflectionTraitCustomSerialization::ReflectionTraitCustomSerialization(const std::function<void(const void*, Document::Node&)> serialization, const std::function<void(void*, const Document::Node&)> deserialization)
	: ReflectionTrait()
	, _serialization(serialization)
	, _deserialization(deserialization)
	{
	}

	/// @brief 
	/// @param instance 
	/// @param documentNode 
	void ReflectionTraitCustomSerialization::Serialize(const void* instance, Document::Node& documentNode) const
	{
		_serialization(instance, documentNode);
	}

	/// @brief 
	/// @param instance 
	/// @param documentNode 
	void ReflectionTraitCustomSerialization::Deserialize(void* instance, const Document::Node& documentNode) const
	{
		_deserialization(instance, documentNode);
	}
}
