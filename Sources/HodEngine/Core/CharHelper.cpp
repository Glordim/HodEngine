#include "HodEngine/Core/CharHelper.hpp"

namespace hod::inline core
{
	/// @brief Words of name (a type or variable name: camelCase, PascalCase, snake_case) separated by spaces, each starting with an upper
	/// case letter: "_velocityScale" -> "Velocity Scale", "UIComponent" -> "UI Component" (a run of capitals is an acronym, its last one
	/// starts the next word when a lower case letter follows), "Rigidbody2dComponent" -> "Rigidbody 2D Component" (a number is a word, a
	/// single letter right after it is its unit: "2d", "3D"), "Vector2Int" -> "Vector 2 Int".
	String GenerateDisplayName(const std::string_view& name)
	{
		String displayName;
		displayName.Reserve((uint32_t)name.size() + 8);

		size_t size = name.size();
		auto   at = [&](size_t index) -> char
		{
			return index < size ? name[index] : '\0';
		};

		bool wordStarted = false; // In a word: the next character may continue it
		for (size_t index = 0; index < size; ++index)
		{
			char c = name[index];
			if (c == '_' || IsWhitespace(c))
			{
				wordStarted = false;
				continue;
			}

			char previous = (index > 0) ? name[index - 1] : '\0';
			char next = at(index + 1);

			bool boundary = false;
			bool upper = false;
			if (wordStarted)
			{
				if (IsUpper(c))
				{
					if (IsLower(previous))
					{
						boundary = true; // camelCase
					}
					else if (IsUpper(previous) && IsLower(next))
					{
						boundary = true; // end of an acronym: "UIComponent"
					}
					else if (IsNumeric(previous))
					{
						// The unit of the number ("2D") unless it starts a word ("2Int")
						boundary = IsLower(next);
					}
				}
				else if (IsNumeric(c))
				{
					boundary = IsAlphabetic(previous); // "Vector2"
				}
				else if (IsLower(c) && IsNumeric(previous) && IsLower(next) == false)
				{
					upper = true; // a single letter after a number: its unit, "2d" -> "2D"
				}
			}

			if (wordStarted == false || boundary)
			{
				if (displayName.Empty() == false)
				{
					displayName += ' ';
				}
				upper = true;
			}
			wordStarted = true;

			displayName += (upper && IsLower(c)) ? ToUpper(c) : c;
		}

		return displayName;
	}
}
