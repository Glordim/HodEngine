#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#undef CreateFont

namespace hod::renderer
{
	class Font;
	class FontManagerImpl;

	class HOD_RENDERER_API FontManager final
	{
		_Singleton(FontManager)

	public:

		~FontManager();

		bool		Init();
		bool		Terminate();

		FontManagerImpl*	GetImpl();

	private:

		FontManagerImpl*	_impl = nullptr;
	};
}
