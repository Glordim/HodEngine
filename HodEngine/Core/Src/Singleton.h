#pragma once

namespace HOD
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<class T, class Tname = T>
		class Singleton
		{
		protected:

							Singleton() = default;
			virtual			~Singleton() = default;

		public:

			static Tname*	CreateInstance();
			static Tname*	GetInstance();
			static void		DestroyInstance();

		protected:

			static T*		_instance;
		};
	}
}

#include "Singleton.inl"
