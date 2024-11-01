#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Importer/Importer.hpp"

namespace hod::game
{
	class SerializedData;
}

namespace hod::editor
{
	class HOD_EDITOR_API SerializedDataAsset
	{
		REFLECTED_CLASS_NO_PARENT(SerializedDataAsset, HOD_EDITOR_API)

	public:

		SerializedDataAsset() = default;
		SerializedDataAsset(game::SerializedData* serializedData);

		game::SerializedData*	GetData();

	private:

		game::SerializedData*	_data = nullptr;
	};
}
