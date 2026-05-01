#pragma once

#include <HodEngine/Application/DesktopApplication.hpp>

namespace hod
{
	class EditorApplication : public DesktopApplication	
	{
	public:
		bool RunInternal() override;

	protected:

		bool InitEditor();
		bool TerminateEditor();
	};
}
