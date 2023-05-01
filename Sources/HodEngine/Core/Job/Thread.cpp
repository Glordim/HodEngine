#include "HodEngine/Core/Job/Thread.h"

#include <Windows.h>

namespace hod
{
	int priorities[Thread::Priority::Count] = { THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL };

	thread_local Thread::Id localThreadId = Thread::InvalidId;

	/// @brief 
	/// @return 
	Thread::Id	Thread::GetCurrentThreadId()
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
		Thread::Function	_function;
		void*				_parameter;
	};

	/// @brief 
	/// @param descriptor 
	/// @return 
	DWORD WINAPI ThreadFunctionInternal(LPVOID param)
	{
		Descriptor* descriptor = static_cast<Descriptor*>(param);

		DWORD exitCode = descriptor->_function(descriptor->_parameter);

		delete descriptor;

		return exitCode;
	}

	/// @brief 
	/// @param function 
	/// @param parameter 
	/// @param priority 
	/// @param name 
	void Thread::Start(const Function& function, void* parameter, Priority priority, const char* name)
	{
		Descriptor* descriptor = new Descriptor();
		descriptor->_function = function;
		descriptor->_parameter = parameter;

		_handle = ::CreateThread(NULL, 0, ThreadFunctionInternal, descriptor, 0, &_id);

		::SetThreadPriority(_handle, priorities[priority]);
		// TODO set affinity ?

		size_t nameLen = strlen(name);
		size_t wNameLen = mbstowcs(NULL, name, nameLen);
		wchar_t* wName = new wchar_t[wNameLen + 1];
		mbstowcs(wName, name, nameLen);
		wName[wNameLen] = '\0';

		::SetThreadDescription(_handle, wName);
		delete[] wName;
	}

	/// @brief 
	void Thread::Join()
	{
		::WaitForMultipleObjects(1, &_handle, TRUE, INFINITE);
	}

	/// @brief 
	void Thread::Yieldd()
	{
		::SwitchToThread();
	}

	/// @brief 
	/// @param millisecond 
	void Thread::Sleep(uint32_t millisecond)
	{
		::Sleep(millisecond);
	}

	/// @brief 
	/// @return 
	Thread::Id Thread::GetId() const
	{
		return _id;
	}
}
