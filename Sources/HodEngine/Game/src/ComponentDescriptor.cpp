#include "HodEngine/Game/src/ComponentDescriptor.h"

namespace hod
{
	namespace game
	{
		/// @brief 
		/// @param metaType 
		/// @param createFunction 
		ComponentDescriptor::ComponentDescriptor(MetaType metaType, const std::string_view& displayName, const CreateFunction& createFunction)
		: _createFunction(createFunction)
		, _metaType(metaType)
		, _displayName(displayName)
		{
		}
	}
}
