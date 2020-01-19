
namespace HOD
{
    template<class T>
    inline T* Singleton<T>::CreateInstance()
    {
        if (T::_instance == nullptr)
        {
            T::_instance = new T();
        }
        return T::_instance;
    }

    template<class T>
    inline T* Singleton<T>::GetInstance()
    {
        return T::_instance;
    }

    template<class T>
    inline void Singleton<T>::DestroyInstance()
    {
        if (T::_instance != nullptr)
        {
            delete T::_instance;
            T::_instance = nullptr;
        }        
    }
}
