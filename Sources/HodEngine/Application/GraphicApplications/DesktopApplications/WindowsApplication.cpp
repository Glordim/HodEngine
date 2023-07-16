#include "HodEngine/Application/GraphicApplications/DesktopApplications/WindowsApplication.h"

#include <ole2.h>

namespace hod::application
{
	_SingletonOverrideConstructor(WindowsApplication)
	{
		OleInitialize(NULL); // TODO Call unintialize
	}
}
