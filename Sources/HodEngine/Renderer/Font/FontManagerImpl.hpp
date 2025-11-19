#pragma once
#include <freetype/freetype.h>

namespace hod::renderer
{
	class Font;

	class FontManagerImpl
	{
	public:

		bool		Init();
		bool		Terminate();

		FT_Library	GetFtLibrary() const;
	
	private:

		FT_Library _ftLibrary = nullptr;
	};
}
