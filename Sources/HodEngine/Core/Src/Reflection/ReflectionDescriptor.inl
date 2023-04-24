
namespace hod
{
	namespace core
	{
		template<typename __TRAIT_TYPE__, typename... Args>
		void ReflectionDescriptor::AddTrait(Args&&... args)
		{
			AddTrait(new __TRAIT_TYPE__(std::forward<Args>(args)...));
		}

		template<typename __PROPERTY_TYPE__, typename... Args>
		void ReflectionDescriptor::AddProperty(Args&&... args)
		{
			AddProperty(new __PROPERTY_TYPE__(std::forward<Args>(args)...));
		}

		template<typename T>
		bool ReflectionDescriptor::SerializeInDocument(const T& instance, Document::Element& documentElement)
		{
			for (ReflectionProperty* property : _properties)
			{
				if (std::strcmp(property->GetTypeName(), "Variable") == 0)
				{
					Reflection::Property::Variable* variable = static_cast<Reflection::Property::Variable*>(property);
					variable->Serialize(instance, documentElement);
				}
			}
			return true;
		}

		template<typename T>
		bool ReflectionDescriptor::DeserializeFromDocument(T& instance, const Document::Element& documentElement)
		{
			for (ReflectionProperty* property : _properties)
			{
				if (std::strcmp(property->GetTypeName(), "Variable") == 0)
				{
					Reflection::Property::Variable* variable = static_cast<Reflection::Property::Variable*>(property);
					variable->Deserialize(instance, documentElement);
				}
			}
			return true;
		}
	}
}
