#pragma once

namespace HOD
{
    template<class T>
    class Singleton
    {
    protected:

                        Singleton() = default;
        virtual         ~Singleton() = default;

    public:

        static T*       CreateInstance();
        static T*       GetInstance();
        static void     DestroyInstance();

    protected:

        static T*       _instance;
    };
}

#include "Singleton.inl"
