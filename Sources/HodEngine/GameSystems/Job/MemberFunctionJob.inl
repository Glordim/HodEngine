
namespace hod::inline gamesystems
{
	template<typename Type, typename Function>
	MemberFunctionJob<Type, Function>::MemberFunctionJob(Type* instance, Function memberFunction, Thread::Id threadId)
	: Job(threadId)
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
