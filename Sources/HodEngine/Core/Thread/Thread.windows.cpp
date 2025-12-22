#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Thread/Thread.hpp"

#include "HodEngine/Core/StringConversion.hpp"

#include <win32/misc.h>
#include <win32/threads.h>

#define THREAD_DYNAMIC_CODE_ALLOW 1 // Opt-out of dynamic code generation.

#define THREAD_BASE_PRIORITY_LOWRT 15   // value that gets a thread to LowRealtime-1
#define THREAD_BASE_PRIORITY_MAX 2      // maximum thread base priority boost
#define THREAD_BASE_PRIORITY_MIN (-2)   // minimum thread base priority boost
#define THREAD_BASE_PRIORITY_IDLE (-15) // value that gets a thread to idle

#define THREAD_PRIORITY_LOWEST THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL (THREAD_PRIORITY_LOWEST + 1)
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL (THREAD_PRIORITY_HIGHEST - 1)
#define THREAD_PRIORITY_ERROR_RETURN (MAXLONG)

#define THREAD_PRIORITY_TIME_CRITICAL THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE THREAD_BASE_PRIORITY_IDLE

#define THREAD_MODE_BACKGROUND_BEGIN 0x00010000
#define THREAD_MODE_BACKGROUND_END 0x00020000

extern "C"
{
	BOOL WINAPI SetThreadPriority(_In_ HANDLE hThread, _In_ int nPriority);
}

namespace hod
{
	int priorities[Thread::Priority::Count] = {THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL};

	thread_local Thread::Id localThreadId = Thread::InvalidId;

	/// @brief
	/// @return
	Thread::Id Thread::GetCurrentThreadId()
	{
		if (localThreadId == Thread::InvalidId)
		{
			localThreadId = ::GetCurrentThreadId();
		}

		return localThreadId;
	}

	/// @brief
	struct Descriptor
	{
		Thread::Function _function;
		void*            _parameter;
	};

	/// @brief
	/// @param descriptor
	/// @return
	DWORD WINAPI ThreadFunctionInternal(LPVOID param)
	{
		Descriptor*      descriptor = static_cast<Descriptor*>(param);
		Thread::Function function = descriptor->_function;
		void*            parameter = descriptor->_parameter;
		DefaultAllocator::GetInstance().Delete(descriptor);

		return function(parameter);
	}

	/// @brief
	/// @param function
	/// @param parameter
	/// @param priority
	/// @param name
	void Thread::Start(const Function& function, void* parameter, Priority priority, const char* name)
	{
		Descriptor* descriptor = DefaultAllocator::GetInstance().New<Descriptor>();
		descriptor->_function = function;
		descriptor->_parameter = parameter;

		_handle = ::CreateThread(NULL, 0, ThreadFunctionInternal, descriptor, 0, &_id);

		::SetThreadPriority(_handle, priorities[priority]);
		// TODO set affinity ?

		std::wstring wideName;
		if (StringConversion::StringToWString(name, wideName) == true)
		{
			::SetThreadDescription(_handle, wideName.c_str());
		}
	}

	/// @brief
	void Thread::Join()
	{
		::WaitForMultipleObjects(1, &_handle, TRUE, INFINITE);
	}

	/// @brief
	void ThisThread::Yield()
	{
		::SwitchToThread();
	}

	/// @brief
	/// @param millisecond
	void ThisThread::Sleep(uint32_t millisecond)
	{
		::Sleep(millisecond);
	}

	void ThisThread::SetName(const char* name)
	{
		std::wstring wideName;
		if (StringConversion::StringToWString(name, wideName) == true)
		{
			::SetThreadDescription(GetCurrentThread(), wideName.c_str());
		}
	}
}
