#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"
#include "HodEngine/Editor/Importer/SerializedDataImporter.hpp"

namespace hod::inline renderer
{
	class MaterialResource;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_EDITOR_API AudioEditorTab : public EditorTab
	{
	public:
				AudioEditorTab(std::shared_ptr<Asset> asset);
				~AudioEditorTab() override;

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
	};
}
