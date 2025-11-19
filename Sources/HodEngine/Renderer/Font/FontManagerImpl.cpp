#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/FontManagerImpl.hpp"

namespace hod::renderer
{
	bool FontManagerImpl::Init()
	{
		FT_Error error = FT_Init_FreeType(&_ftLibrary);
		if (error != 0)
		{
			OUTPUT_ERROR("FreeType::FT_Init_FreeType {}", FT_Error_String(error));
			return false;
		}
		return true;
	}

	bool FontManagerImpl::Terminate()
	{
		FT_Error error = FT_Done_FreeType(_ftLibrary);
		if (error != 0)
		{
			OUTPUT_ERROR("FreeType::FT_Done_FreeType {}", FT_Error_String(error));
			return false;
		}
		return true;
	}

	FT_Library FontManagerImpl::GetFtLibrary() const
	{
		return _ftLibrary;
	}
}
