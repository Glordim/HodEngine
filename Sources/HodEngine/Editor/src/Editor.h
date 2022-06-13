#pragma once

#include <HodEngine/Core/Src/Singleton.h>
#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>

#include <stdint.h>

namespace HOD
{
	class Window;

	/// @brief 
	class Editor : public Singleton<Editor>
	{
		friend class Singleton<Editor>;

	public:

		bool							Init(Window* window);
		void							Update();

	protected:

										Editor();
										~Editor();

	private:

		MemberFunctionJob<Editor>		_updateJob;

	};
}
