#pragma once

#include <HodEngine/Core/Src/Singleton.h>
#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>

#include <stdint.h>

namespace HOD
{
	/// @brief 
	class Editor : public Singleton<Editor>
	{
		friend class Singleton<Editor>;

	public:

		void							Update();

	protected:

										Editor();

	private:

		MemberFunctionJob<Editor>		_updateJob;

	};
}
