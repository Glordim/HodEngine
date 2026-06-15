
namespace hod::inline gamesystems
{
	template<typename Type, typename Function>
	MemberFunctionJob<Type, Function>::MemberFunctionJob(Type* instance, Function memberFunction)
	: Job()
	, _instance(instance)
	, _memberFunction(memberFunction)
	{}

	template<typename Type, typename Function>
	inline void MemberFunctionJob<Type, Function>::Execution()
	{
		(_instance->*_memberFunction)();
	}
}
