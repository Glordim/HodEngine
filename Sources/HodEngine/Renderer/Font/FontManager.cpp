#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Font/FontManager.hpp"
#include "HodEngine/Renderer/Font/FontManagerImpl.hpp"

namespace hod::renderer
{
	_SingletonConstructor(FontManager)
	{
		_impl = DefaultAllocator::GetInstance().New<FontManagerImpl>();
	}

	FontManager::~FontManager()
	{
		DefaultAllocator::GetInstance().Delete(_impl);
	}

	bool FontManager::Init()
	{
		return _impl->Init();
	}

	bool FontManager::Terminate()
	{
		return _impl->Terminate();
	}

	FontManagerImpl* FontManager::GetImpl()
	{
		return _impl;
	}
}
