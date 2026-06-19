#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string_view>

namespace hod::inline editor
{
	enum class TaskStatus
	{
		Pending,
		Running,
		Succeeded,
		Failed,
		Canceled,
	};

	struct Task
	{
		uint64_t id;
		String name;
		String description;
		float startTime = -1.0f;
		float remainingTime = -1.0f;
		float percent = -1.0f;
		TaskStatus status = TaskStatus::Pending;
		std::function<bool()> cancelCallback;
	};

	/// @brief
	class HOD_EDITOR_API TaskTracker
	{
	public:
		TaskTracker() = default;
		~TaskTracker() = default;

	public:
		[[nodiscard]] uint64_t CreateTask(std::string_view name, std::string_view description, const std::function<bool()>& cancelCallback = nullptr);

		bool	UpdateTaskName(uint64_t id, std::string_view name);
		bool	UpdateTaskDescription(uint64_t id, std::string_view description);
		bool	UpdateTaskProgress(uint64_t id, float percent, float remainingTime = -1.0f);
		bool	UpdateTaskStatus(uint64_t id, TaskStatus status);

		bool	RemoveTask(uint64_t id);

		void				LockTasks() { _taskLock.lock(); }
		const Vector<Task*>	GetTasks() const { return _tasks; }
		void				UnlockTasks() { _taskLock.unlock(); }

	private:
		[[nodiscard]] Task* FindTask(uint64_t id) const;

	private:
		std::atomic<uint64_t>	_nextTaskId = { 0 };

		std::mutex				_taskLock;
		Vector<Task*>			_tasks;
	};
}
