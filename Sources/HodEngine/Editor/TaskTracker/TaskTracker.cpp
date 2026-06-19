#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"

namespace hod::inline editor
{
	Task* TaskTracker::FindTask(uint64_t id) const
	{
		for (uint32_t i = 0; i < _tasks.Size(); ++i)
		{
			if (_tasks[i]->id == id)
			{
				return _tasks[i];
			}
		}
		return nullptr;
	}

	uint64_t TaskTracker::CreateTask(std::string_view name, std::string_view description, const std::function<bool()>& cancelCallback)
	{
		uint64_t id = _nextTaskId.fetch_add(1);

		Task* task = DefaultAllocator::GetInstance().New<Task>();
		task->id = id;
		task->name = name;
		task->description = description;
		task->status = TaskStatus::Pending;
		task->cancelCallback = cancelCallback;

		std::scoped_lock lock(_taskLock);
		_tasks.PushBack(task);

		return id;
	}

	bool TaskTracker::UpdateTaskName(uint64_t id, std::string_view name)
	{
		std::scoped_lock lock(_taskLock);
		Task* task = FindTask(id);
		if (task == nullptr)
		{
			return false;
		}
		task->name = name;
		return true;
	}

	bool TaskTracker::UpdateTaskDescription(uint64_t id, std::string_view description)
	{
		std::scoped_lock lock(_taskLock);
		Task* task = FindTask(id);
		if (task == nullptr)
		{
			return false;
		}
		task->description = description;
		return true;
	}

	bool TaskTracker::UpdateTaskProgress(uint64_t id, float percent, float remainingTime)
	{
		std::scoped_lock lock(_taskLock);
		Task* task = FindTask(id);
		if (task == nullptr)
		{
			return false;
		}
		task->percent = percent;
		task->remainingTime = remainingTime;
		return true;
	}

	bool TaskTracker::UpdateTaskStatus(uint64_t id, TaskStatus status)
	{
		std::scoped_lock lock(_taskLock);
		Task* task = FindTask(id);
		if (task == nullptr)
		{
			return false;
		}
		task->status = status;
		return true;
	}

	bool TaskTracker::RemoveTask(uint64_t id)
	{
		std::scoped_lock lock(_taskLock);
		for (uint32_t i = 0; i < _tasks.Size(); ++i)
		{
			if (_tasks[i]->id == id)
			{
				DefaultAllocator::GetInstance().Delete(_tasks[i]);
				_tasks.Erase(i);
				return true;
			}
		}
		return false;
	}
}
