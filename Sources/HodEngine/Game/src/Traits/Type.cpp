#include "Type.h"

namespace hod
{
	namespace game
	{
		namespace Traits
		{
			Type::Type(TypeEnum type)
			: core::ReflectionTrait("Type")
			, _type(type)
			{

			}

			rapidjson::Value Type::Serialize(rapidjson::Document::AllocatorType& allocator) const
			{
				rapidjson::Value trait;
				trait.SetObject();

				trait.AddMember("TypeName", rapidjson::StringRef(_typeName), allocator);
				trait.AddMember("Type", rapidjson::StringRef(_typeLabels[_type]), allocator);

				return trait;
			};
		}
	}
}
