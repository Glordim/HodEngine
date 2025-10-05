#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <cstdint>

#if defined(PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#undef Yield
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	#include <pthread.h>
#endif

namespace hod
{
	/// @brief
	class HOD_CORE_API Thread
	{
	public:
#if defined(PLATFORM_WINDOWS)
		using Id = DWORD;
		static constexpr Id InvalidId = 0;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS) || defined(PLATFORM_ANDROID)
		using Id = pthread_t;
		static constexpr Id InvalidId = 0;
#endif

		using Function = int (*)(void*);

		enum Priority
		{
			Low = 0,
			Normal,
			High,

			Count
		};

	public:
		static Id GetCurrentThreadId();

	public:
		Thread() = default;
		Thread(const Thread&) = delete;
		Thread(Thread&&) = delete;
		~Thread() = default;

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) = delete;

	public:
		void Start(const Function& function, void* parameter = nullptr, Priority priority = Priority::Normal, const char* name = nullptr);
		void Join();
		Id   GetId() const;

	private:
		Id _id;

#if defined(PLATFORM_WINDOWS)
		HANDLE _handle;
#endif
	};

	/// @brief
	class HOD_CORE_API ThisThread
	{
	public:
		static void Yield();
		static void Sleep(uint32_t millisecond);
	};
}
