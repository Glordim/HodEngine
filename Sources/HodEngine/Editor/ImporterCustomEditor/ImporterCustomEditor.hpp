#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <memory>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::editor
{
	class Asset;
	
	///@brief 
	class HOD_EDITOR_API ImporterCustomEditor
	{
	public:

									ImporterCustomEditor() = default;
									ImporterCustomEditor(const ImporterCustomEditor&) = delete;
									ImporterCustomEditor(ImporterCustomEditor&&) = delete;
			virtual					~ImporterCustomEditor() = default;

			ImporterCustomEditor	operator = (const ImporterCustomEditor&) = delete;
			ImporterCustomEditor	operator = (ImporterCustomEditor&&) = delete;

	public:

		virtual void				OnInit(std::shared_ptr<Asset> asset);
		virtual bool				OnDrawInspector();

	protected:

		std::shared_ptr<Asset>		_asset = nullptr;
	};
}
