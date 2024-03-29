
namespace hod
{
	template<typename T>
	T* Singleton<T>::_instance = nullptr;

	/// @brief 
	/// @tparam T 
	/// @tparam Tname 
	/// @return 
	template<typename T>
	inline T* Singleton<T>::CreateInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new T();
		}
		return (T*)_instance;
	}

	/// @brief 
	/// @tparam T 
	/// @tparam Tname 
	/// @return 
	template<typename T>
	inline T* Singleton<T>::GetInstance()
	{
		return (T*)_instance;
	}

	/// @brief 
	/// @tparam T 
	/// @tparam Tname 
	template<typename T>
	inline void Singleton<T>::DestroyInstance()
	{
		if (T::_instance != nullptr)
		{
			delete T::_instance;
			T::_instance = nullptr;
		}
	}
}
