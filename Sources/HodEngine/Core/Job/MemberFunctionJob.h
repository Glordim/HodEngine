#pragma once

#include "HodEngine/Core/Job/Job.h"

namespace hod
{
	/// @brief 
	template<typename Type, typename Function = void (Type::*)()>
	class MemberFunctionJob : public Job
	{
	public:

							MemberFunctionJob(Type* instance, Function memberFunction, JobQueue::Queue queue, bool deleteAfterCompletion = false, Thread::Id threadId = Thread::InvalidId);
							MemberFunctionJob(const Job&) = delete;
							MemberFunctionJob(Job&&) = delete;
							~MemberFunctionJob() = default;

		MemberFunctionJob&	operator = (const MemberFunctionJob&) = delete;
		MemberFunctionJob&	operator = (MemberFunctionJob&&) = delete;

	protected:

		void				Execution() override;

	private:

		Type*				_instance;
		Function			_memberFunction;
	};
}

#include "MemberFunctionJob.inl"
