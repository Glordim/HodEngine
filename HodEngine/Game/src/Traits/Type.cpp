#include "Type.h"

namespace HOD
{
	namespace GAME
	{
		namespace Traits
		{
			Type::Type(TypeEnum type)
			: CORE::ReflectionTrait("Type")
			, _type(type)
			{

			}

			void Type::Serialize(rapidjson::Document& document) const
			{
				document.StartObject();

				document.String("Type", 4, false);
				document.String(_typeLabels[_type], std::strlen(_typeLabels[_type]), false);

				document.EndObject(1);
			};
		}
	}
}