#include "HodEngine/Core/Job/Thread.hpp"

#if defined(PLATFORM_WINDOWS)
#include "HodEngine/Core/StringConversion.hpp"
#endif
#if defined(PLATFORM_LINUX)
#include <unistd.h>
#endif

namespace hod
{
#if defined(PLATFORM_WINDOWS)
	int priorities[Thread::Priority::Count] = { THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL };
#elif defined(PLATFORM_LINUX)
	int priorities[Thread::Priority::Count] = { -1, 0, 1 };
#endif

	thread_local Thread::Id localThreadId = Thread::InvalidId;

	/// @brief 
	/// @return 
	Thread::Id Thread::GetCurrentThreadId()
	{
		if (localThreadId == Thread::InvalidId)
		{
#if defined(PLATFORM_WINDOWS)
			localThreadId = ::GetCurrentThreadId();
#elif defined(PLATFORM_LINUX)
			localThreadId = pthread_self();
#endif
		}

		return localThreadId;
	}

	/// @brief 
	struct Descriptor
	{
		Thread::Function	_function;
		void*				_parameter;
	};

#if defined(PLATFORM_WINDOWS)
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
#elif defined(PLATFORM_LINUX)
	/// @brief 
	/// @param descriptor 
	/// @return 
	void* ThreadFunctionInternal(void* param)
	{
		Descriptor* descriptor = static_cast<Descriptor*>(param);

		int exitCode = descriptor->_function(descriptor->_parameter);

		delete descriptor;

		pthread_exit(&exitCode);
	}
#endif

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

#if defined(PLATFORM_WINDOWS)
		_handle = ::CreateThread(NULL, 0, ThreadFunctionInternal, descriptor, 0, &_id);

		::SetThreadPriority(_handle, priorities[priority]);
		// TODO set affinity ?

		std::wstring wideName;
		if (StringConversion::StringToWString(name, wideName) == true)
		{
			::SetThreadDescription(_handle, wideName.c_str());
		}		
#elif defined(PLATFORM_LINUX)
		pthread_attr_t attributes;
		if (pthread_attr_init(&attributes) != 0)
		{
			// todo error
			return;
		}

		// todo set attributes

		if (pthread_create(&_id, &attributes, ThreadFunctionInternal, descriptor) != 0)
		{
			pthread_attr_destroy(&attributes);
			// todo error
			return;
		}

		if (pthread_attr_destroy(&attributes) != 0)
		{
			// todo error
		}
#endif
	}

	/// @brief 
	void Thread::Join()
	{
#if defined(PLATFORM_WINDOWS)
		::WaitForMultipleObjects(1, &_handle, TRUE, INFINITE);
#elif defined(PLATFORM_LINUX)
		if (pthread_join(_id, nullptr) != 0)
		{
			// todo error
		}
#endif
	}

	/// @brief 
	void Thread::Yieldd()
	{
#if defined(PLATFORM_WINDOWS)
		::SwitchToThread();
#elif defined(PLATFORM_LINUX)
		pthread_yield(); // deprecated use sched_yield instead
#endif
	}

	/// @brief 
	/// @param millisecond 
	void Thread::Sleep(uint32_t millisecond)
	{
#if defined(PLATFORM_WINDOWS)
		::Sleep(millisecond);
#elif defined(PLATFORM_LINUX)
		usleep(millisecond * 1000);
#endif
	}

	/// @brief 
	/// @return 
	Thread::Id Thread::GetId() const
	{
		return _id;
	}
}
