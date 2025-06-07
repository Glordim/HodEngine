#pragma once
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

#define _SingletonAbstract(T)															\
	public:																				\
																						\
		static T*		GetInstance() { return _instance; }								\
																						\
	protected:																			\
																						\
						T();															\
																						\
	public:																				\
																						\
						T(const T&) = delete;											\
						T(T&&) = delete;												\
																						\
		T&				operator = (const T&) = delete;									\
		T&				operator = (T&&) = delete;										\
																						\
	protected:																			\
																						\
		static T*		_instance;														\
																						\
	private:																			\

#define _SingletonAbstractOverride(T)													\
	public:																				\
																						\
		static T*		GetInstance() { return static_cast<T*>(_instance); }			\
																						\
	protected:																			\
																						\
						T();															\
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

// todo CreateInstance check if alreay exist #memleak

#define _Singleton(T)																	\
		friend class hod::Allocator;													\
																						\
	public:																				\
																						\
		static T*		CreateInstance() { _instance = hod::DefaultAllocator::GetInstance().New<T>(); return _instance; }		\
		static T*		GetInstance() { return _instance; }								\
		static void		DestroyInstance() { hod::DefaultAllocator::GetInstance().Delete(_instance); _instance = nullptr; }	\
																						\
	protected:																			\
																						\
						T();															\
																						\
	public:																				\
																						\
						T(const T&) = delete;											\
						T(T&&) = delete;												\
																						\
		T&				operator = (const T&) = delete;									\
		T&				operator = (T&&) = delete;										\
																						\
	protected:																			\
																						\
		static T*		_instance;														\
																						\
	private:																			\
																						

#define _SingletonConstructor(T)	\
	T* T::_instance = nullptr;		\
									\
	T::T()							

#define _SingletonOverride(T)																			\
		friend class hod::Allocator;																	\
																										\
	public:																								\
																										\
		static T*		CreateInstance() { _instance = hod::DefaultAllocator::GetInstance().New<T>(); return static_cast<T*>(_instance); }	\
		static T*		GetInstance() { return static_cast<T*>(_instance); }							\
		static void		DestroyInstance() { hod::DefaultAllocator::GetInstance().Delete(_instance); _instance = nullptr; }					\
																										\
	protected:																							\
																										\
						T();																			\
																										\
	public:																								\
																										\
						T(const T&) = delete;															\
						T(T&&) = delete;																\
																										\
		T&				operator = (const T&) = delete;													\
		T&				operator = (T&&) = delete;														\
																										\
	private:																							\


#define _SingletonOverrideConstructor(T)	\
	T::T()									

namespace hod
{
	/// @brief 
	/// @tparam T 
	/// @tparam Tname 
	template<typename T>
	class Singleton
	{
		friend T;
		friend class Allocator;

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
