#pragma once

#include <vector>

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

namespace hod
{
	/// @brief 
	class Thread
	{
	public:

#if defined(PLATFORM_WINDOWS)
		using Id = DWORD;
		static constexpr Id InvalidId = 0;
#endif

		using Function = int(*)(void*);

		enum Priority
		{
			Low = 0,
			Normal,
			High,

			Count
		};

	public:

		static Id	GetCurrentThreadId();

	public:

					Thread() = default;
					Thread(const Thread&) = delete;
					Thread(Thread&&) = delete;
					~Thread() = default;

		Thread&		operator = (const Thread&) = delete;
		Thread&		operator = (Thread&&) = delete;

	public:

		void		Start(const Function& function, void* parameter = nullptr, Priority priority = Priority::Normal, const char* name = nullptr);
		void		Join();
		void		Yieldd();
		void		Sleep(uint32_t millisecond);
		Id			GetId() const;

	private:

		Id			_id;

#if defined(PLATFORM_WINDOWS)
		HANDLE		_handle;
#endif
	};
}
