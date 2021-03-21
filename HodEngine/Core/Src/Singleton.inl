
namespace HOD
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<class T, class Tname>
		inline Tname* Singleton<T, Tname>::CreateInstance()
		{
			if (Tname::_instance == nullptr)
			{
				Tname::_instance = (T*)new Tname();
			}
			return (Tname*)Tname::_instance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<class T, class Tname>
		inline Tname* Singleton<T, Tname>::GetInstance()
		{
			return (Tname*)Tname::_instance;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<class T, class Tname>
		inline void Singleton<T, Tname>::DestroyInstance()
		{
			if (Tname::_instance != nullptr)
			{
				delete Tname::_instance;
				Tname::_instance = nullptr;
			}
		}
	}
}
