#pragma once
#include "HodEngine/Core/Thread/Thread.hpp"
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Core/Singleton.hpp"
#include <cstdint>

namespace hod::inline window
{
	class Window;

	class HOD_WINDOW_API DisplayManager
	{
		_SingletonAbstract(DisplayManager)

	public:
		virtual ~DisplayManager() = default;

		virtual bool Initialize() = 0;
		virtual void Update() = 0;
		virtual void Terminate() = 0;

		Window* GetMainWindow() const;

		void SetThreadId(uint32_t threadId)
		{
			_threadId = threadId;
		}

		uint32_t GetThreadId() const
		{
			return _threadId;
		}

	protected:
		Window*  _mainWindow = nullptr;
		uint32_t _threadId = 0;
	};
}
