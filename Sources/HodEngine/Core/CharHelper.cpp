#include "HodEngine/Core/CharHelper.hpp"

namespace hod
{
	std::string GenerateDisplayName(const std::string_view& name)
	{
		std::string displayName;
		displayName.reserve(name.size());

		bool nextWord = true;
		for (char c : name)
		{
			if (c == '_')
			{
				if (displayName.empty() == false)
				{
					displayName += ' ';
					nextWord = true;
				}
				continue;
			}
			else if (nextWord == false && IsUpper(c) == true)
			{
				displayName += ' ';
				displayName += c;
			}
			else
			{
				if (nextWord == true && IsLower(c) == true)
				{
					c = ToUpper(c);
				}
				nextWord = false;
				displayName += c;
			}
		}

		return displayName;
	}
}
