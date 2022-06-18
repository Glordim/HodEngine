
namespace hod
{
	template<typename Type, typename Function>
	MemberFunctionJob<Type, Function>::MemberFunctionJob(Type* instance, Function memberFunction, JobQueue::Queue queue, bool deleteAfterCompletion, Thread::Id threadId)
		: Job(queue, deleteAfterCompletion, threadId)
		, _instance(instance)
		, _memberFunction(memberFunction)
	{

	}

	template<typename Type, typename Function>
	inline void MemberFunctionJob<Type, Function>::Execution()
	{
		(_instance->*_memberFunction)();
	}
}
