#include "ReflectionDescriptor.h"

#include "ReflectionTrait.h"

namespace HOD
{
	namespace CORE
	{
		///@brief Construct a new ReflectionDescriptor::ReflectionDescriptor object
		///@param typeName 
		ReflectionDescriptor::ReflectionDescriptor(const char* typeName)
		: _typeName(typeName)
		{

		}

		void ReflectionDescriptor::Serialize(rapidjson::Document& document) const
		{
			document.String("TypeName", static_cast<rapidjson::SizeType>(9), false);
			document.String(_typeName, static_cast<rapidjson::SizeType>(std::strlen(_typeName)), false);

			document.String("Traits", static_cast<rapidjson::SizeType>(7), false);
			document.StartArray();
			int count = 0;
			for (const ReflectionTrait* trait : _traits)
			{
				trait->Serialize(document);
				count++;
			}
			document.EndArray(count);

/* PROPERYTY
			document.String("Traits", static_cast<rapidjson::SizeType>(7), false);
			document.StartArray();
			int count = 0;
			for (const ReflectionTrait* trait : _traits)
			{
				trait->Serialize(document);
				count++;
			}
			document.EndArray(count);
*/
		}

		void ReflectionDescriptor::Deserialize()
		{
			// TODO
		}

		///@brief 
		///@return const std::vector<ReflectionTrait*>& 
		const std::vector<ReflectionTrait*>& ReflectionDescriptor::GetTraits() const
		{
			return _traits;
		}

		///@brief 
		///@return const std::vector<ReflectionProperty*>& 
		const std::vector<ReflectionProperty*>& ReflectionDescriptor::GetProperties() const
		{
			return _properties;
		}

		///@brief 
		///@param trait 
		void ReflectionDescriptor::AddTrait(ReflectionTrait* trait)
		{
			_traits.push_back(trait);
		}

		///@brief 
		///@param property 
		void ReflectionDescriptor::AddProperty(ReflectionProperty* property)
		{
			_properties.push_back(property);
		}
	}
}
