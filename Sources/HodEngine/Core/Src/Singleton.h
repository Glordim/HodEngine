#pragma once

#define _Singleton(T)																	\
	public:																				\
																						\
		static T*		CreateInstance() { _instance = new T(); return _instance; }		\
		static T*		GetInstance() { return _instance; }								\
		static void		DestroyInstance() { delete _instance; _instance = nullptr; }	\
																						\
	public:																				\
																						\
						T(const T&) = delete;											\
						T(T&&) = delete;												\
																						\
		T&				operator = (const T&) = delete;									\
		T&				operator = (T&&) = delete;										\
																						\
	private:																			\
																						\
		static T*		_instance;														\
																						\
	protected:																			\
																						

#define _SingletonInstance(T) T* T::_instance = nullptr;

namespace HOD
{
	/// @brief 
	/// @tparam T 
	/// @tparam Tname 
	template<typename T>
	class Singleton
	{
		friend typename T;

	public:

		static T*		CreateInstance();
		static T*		GetInstance();
		static void		DestroyInstance();

	public:

						Singleton(const Singleton&) = delete;
						Singleton(Singleton&&) = delete;

		Singleton&		operator = (const Singleton&) = delete;
		Singleton&		operator = (Singleton&&) = delete;

	protected:

						Singleton() = default;
		virtual			~Singleton() = default;

	protected:

		static T*		_instance;
	};
}

#include "Singleton.inl"
