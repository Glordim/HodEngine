#include "ReflectionPropertyVariable.h"

namespace hod
{
	namespace CORE
	{
		namespace Reflection
		{
			namespace Property
			{
				Variable::Variable(Type type, uint32_t offset, const char* name)
					: ReflectionProperty("Variable")
					, _type(type)
					, _offset(offset)
					, _name(name)
				{

				}

				rapidjson::Value Variable::Serialize(rapidjson::Document::AllocatorType& allocator) const
				{
					rapidjson::Value descriptor;
					descriptor.SetObject();

					descriptor.AddMember("TypeName", rapidjson::StringRef(_typeName), allocator);
					descriptor.AddMember("Name", rapidjson::StringRef(_name), allocator);
					descriptor.AddMember("Type", rapidjson::StringRef(_typeLabels[_type]), allocator);
					descriptor.AddMember("Offset", _offset, allocator);

					return descriptor;
				}
			}
		}
	}
}
