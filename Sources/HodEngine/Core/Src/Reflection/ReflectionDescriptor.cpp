#include "ReflectionDescriptor.h"

#include "ReflectionTrait.h"
#include "ReflectionProperty.h"

namespace HOD
{
	namespace CORE
	{
		///@brief Construct a new ReflectionDescriptor::ReflectionDescriptor object
		///@param typeName 
		ReflectionDescriptor::ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent)
		: _typeName(typeName)
		, _parent(parent)
		{

		}

		rapidjson::Value ReflectionDescriptor::Serialize(rapidjson::Document::AllocatorType& allocator) const
		{
			rapidjson::Value description;
			description.SetObject();

			description.AddMember("TypeName", rapidjson::StringRef(_typeName), allocator);
			if (_parent != nullptr)
			{
				description.AddMember("ParentTypeName", rapidjson::StringRef(_parent->_typeName), allocator);
			}

			rapidjson::Value traits;
			traits.SetArray();

			for (const ReflectionTrait* trait : _traits)
			{
				traits.PushBack(trait->Serialize(allocator), allocator);
			}

			description.AddMember("Traits", traits, allocator);

			rapidjson::Value properties;
			properties.SetArray();

			for (const ReflectionProperty* property : _properties)
			{
				properties.PushBack(property->Serialize(allocator), allocator);
			}

			description.AddMember("Properties", properties, allocator);

			return description;
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