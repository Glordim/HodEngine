#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/StringView.hpp"

#include "HodEngine/Core/String.hpp"

namespace hod::inline core
{
	StringView::StringView(const String& string) noexcept
	: _data(string.CStr())
	, _size(string.Size())
	{
	}
}
