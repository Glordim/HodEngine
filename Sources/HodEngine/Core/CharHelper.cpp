#include "HodEngine/Core/CharHelper.hpp"

namespace hod
{
	String GenerateDisplayName(const std::string_view& name)
	{
		String displayName;
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
