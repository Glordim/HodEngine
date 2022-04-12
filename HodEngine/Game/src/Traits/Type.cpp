#include "Type.h"

namespace HOD
{
	namespace GAME
	{
		namespace Traits
		{
			Type::Type(TypeEnum type)
			: _type(type)
			{

			}

			void Type::Serialize(rapidjson::Document& document)
			{
				document.StartObject();

				document.String("Type", 4, false);
				document.String(_typeLabels[_type], std::strlen(_typeLabels[_type]), false);

				document.EndObject(1);
			};
		}
	}
}