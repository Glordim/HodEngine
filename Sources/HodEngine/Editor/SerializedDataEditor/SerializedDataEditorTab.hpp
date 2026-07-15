#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <memory>
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Game/SerializedDataContainer.hpp"

namespace hod::inline editor
{
	/// @brief
	class HOD_EDITOR_API SerializedDataEditorTab : public EditorTab
	{
	public:
				SerializedDataEditorTab(std::shared_ptr<Asset> asset);
				~SerializedDataEditorTab() override;

		SerializedDataContainer&	GetSerializedDataContainer();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		SerializedDataContainer	_serializedDataContainer;
	};
}
