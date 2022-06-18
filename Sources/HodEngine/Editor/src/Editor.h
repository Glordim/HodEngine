#pragma once

#include <stdint.h>

#include <HodEngine/Core/Src/Singleton.h>

namespace hod::editor
{
	class MainBar;

	/// @brief 
	class Editor : public Singleton<Editor>
	{
		friend class Singleton<Editor>;

	public:

		bool		Init();

	protected:

					Editor();
					~Editor();

	private:

		MainBar*	_mainBar = nullptr;
	};
}
