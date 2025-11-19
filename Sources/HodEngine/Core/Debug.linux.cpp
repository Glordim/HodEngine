#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Assert.hpp"

namespace hod
{
	bool IsDebuggerAttached()
	{
		FILE* file = fopen("/proc/self/status", "r");
		if (!file)
		{
			return false;
		}

		char line[256];
		while (fgets(line, sizeof(line), file))
		{
			if (strncmp(line, "TracerPid:", 10) == 0)
			{
				int tracerPid = atoi(line + 10);
				fclose(file);
				return tracerPid != 0;
			}
		}

		fclose(file);
		return false;
	}
}
