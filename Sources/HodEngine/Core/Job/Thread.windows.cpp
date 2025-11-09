#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Job/Thread.hpp"

#include "HodEngine/Core/StringConversion.hpp"

// #include <win32/threads.h>
// #include <win32/threads.h>
#include <Windows.h>
#undef Yield

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
}
