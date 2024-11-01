#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ImporterCustomEditor/ImporterCustomEditor.hpp"

namespace hod::editor
{
	/// @brief 
	void ImporterCustomEditor::OnInit(std::shared_ptr<Asset> asset)
	{
		_asset = asset;
	}

	/// @brief 
	bool ImporterCustomEditor::OnDrawInspector()
	{
		return false;
	}
}
