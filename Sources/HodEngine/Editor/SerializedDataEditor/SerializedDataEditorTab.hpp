#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"
#include "HodEngine/Editor/Importer/SerializedDataImporter.hpp"

namespace hod::renderer
{
	class MaterialResource;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API SerializedDataEditorTab : public EditorTab
	{
	public:
				SerializedDataEditorTab(std::shared_ptr<Asset> asset);
				~SerializedDataEditorTab() override;

		SerializedDataAsset&	GetSerializedDataAsset();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;

	private:

		SerializedDataAsset	_serializedDataAsset;
	};
}
