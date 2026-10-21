#pragma once

#include <Foundation/NSAutoreleasePool.hpp>

namespace hod::inline rhi
{
	/// @brief Releases, when it goes out of scope, what the Metal calls made in it autoreleased: no pool is drained around the frames of the engine.
	/// What a Metal call returns without "new", "alloc" or "copy" in its name is autoreleased: retain() it to keep it past the scope.
	class MetalAutoreleaseScope
	{
	public:
		MetalAutoreleaseScope()
		: _pool(NS::AutoreleasePool::alloc()->init())
		{
		}

		~MetalAutoreleaseScope()
		{
			_pool->release();
		}

		MetalAutoreleaseScope(const MetalAutoreleaseScope&) = delete;
		MetalAutoreleaseScope(MetalAutoreleaseScope&&) = delete;
		MetalAutoreleaseScope& operator=(const MetalAutoreleaseScope&) = delete;
		MetalAutoreleaseScope& operator=(MetalAutoreleaseScope&&) = delete;

	private:
		NS::AutoreleasePool* _pool = nullptr;
	};
}
