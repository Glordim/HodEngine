
namespace hod
{
	namespace core
	{
		template<typename __TRAIT_TYPE__, typename... Args>
		__TRAIT_TYPE__* ReflectionDescriptor::AddTrait(Args&&... args)
		{
			__TRAIT_TYPE__* trait = new __TRAIT_TYPE__(std::forward<Args>(args)...);
			AddTrait(trait);
			return trait;
		}

		template<typename __PROPERTY_TYPE__, typename... Args>
		__PROPERTY_TYPE__* ReflectionDescriptor::AddProperty(Args&&... args)
		{
			__PROPERTY_TYPE__* property = new __PROPERTY_TYPE__(std::forward<Args>(args)...);
			AddProperty(property);
			return property;
		}

		template<typename T>
		bool ReflectionDescriptor::SerializeInDocument(const T& instance, Document::Node& documentNode)
		{
			return SerializeInDocument(reinterpret_cast<const void*>(&instance), documentNode);
		}

		template<typename T>
		bool ReflectionDescriptor::DeserializeFromDocument(T& instance, const Document::Node& documentNode)
		{
			return DeserializeFromDocument(reinterpret_cast<void*>(&instance), documentNode);
		}
	}
}
