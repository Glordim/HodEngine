#pragma once

#include "HodEngine/GameSystems/Job/Job.hpp"

namespace hod::inline gamesystems
{
	/// @brief
	template<typename Type, typename Function = void (Type::*)()>
	class MemberFunctionJob : public Job
	{
	public:
		MemberFunctionJob(Type* instance, Function memberFunction);
		MemberFunctionJob(const Job&) = delete;
		MemberFunctionJob(Job&&) = delete;
		~MemberFunctionJob() override = default;

		MemberFunctionJob& operator=(const MemberFunctionJob&) = delete;
		MemberFunctionJob& operator=(MemberFunctionJob&&) = delete;

	protected:
		void Execution() override;

	private:
		Type*    _instance;
		Function _memberFunction;
	};
}

#include "MemberFunctionJob.inl"
