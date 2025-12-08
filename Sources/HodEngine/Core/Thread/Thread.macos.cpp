#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Thread/Thread.hpp"

#include <unistd.h>

namespace hod
{
	int priorities[Thread::Priority::Count] = {-1, 0, 1};

	thread_local Thread::Id localThreadId = Thread::InvalidId;

	/// @brief
	/// @return
	Thread::Id Thread::GetCurrentThreadId()
	{
		if (localThreadId == Thread::InvalidId)
		{
			localThreadId = pthread_self();
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
	void* ThreadFunctionInternal(void* param)
	{
		Descriptor*      descriptor = static_cast<Descriptor*>(param);
		Thread::Function function = descriptor->_function;
		void*            parameter = descriptor->_parameter;
		DefaultAllocator::GetInstance().Delete(descriptor);

		int exitCode = function(parameter);

		pthread_exit(&exitCode);
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

		pthread_attr_t attributes;
		if (pthread_attr_init(&attributes) != 0)
		{
			// todo error
			return;
		}

		// todo set attributes
		(void)priority;
		(void)name;

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
	}

	/// @brief
	void Thread::Join()
	{
		if (pthread_join(_id, nullptr) != 0)
		{
			// todo error
		}
	}

	/// @brief
	void ThisThread::Yield()
	{
		sched_yield();
	}

	/// @brief
	/// @param millisecond
	void ThisThread::Sleep(uint32_t millisecond)
	{
		usleep(millisecond * 1000);
	}
}
