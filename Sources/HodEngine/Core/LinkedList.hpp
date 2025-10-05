#pragma once

#include <atomic>
#include <stdint.h>

namespace hod
{
	/// @brief
	/// @tparam T
	/// @tparam Tname
	template<typename Type>
	class LinkedList
	{
	public:
		class Iterator
		{
		public:
			Iterator(LinkedList<Type>* reference)
			: _reference(reference)
			{
			}

			bool operator==(const Iterator& rhs) const
			{
				return _reference == rhs._reference;
			}

			bool operator!=(const Iterator& rhs) const
			{
				return _reference != rhs._reference;
			}

			Iterator& operator++()
			{
				_reference = _reference->Next();
				return *this;
			}

			Type* operator*()
			{
				return _reference->Data();
			}

			/*
			LinkedList<Type>* operator * ()
			{
			    return _reference;
			}
			*/

		private:
			LinkedList<Type>* _reference = nullptr;
		};

	public:
		LinkedList();
		LinkedList(Type* data);
		LinkedList(const LinkedList&) = default;
		LinkedList(LinkedList&&) = default;
		~LinkedList() = default;

		LinkedList& operator=(const LinkedList&) = default;
		LinkedList& operator=(LinkedList&&) = default;

	public:
		LinkedList<Type>::Iterator begin()
		{
			return Iterator(this);
		}

		LinkedList<Type>::Iterator end()
		{
			return Iterator(nullptr);
		}

		LinkedList<Type>::Iterator begin() const
		{
			return Iterator((LinkedList<Type>*)(this));
		}

		LinkedList<Type>::Iterator end() const
		{
			return Iterator(nullptr);
		}

		const LinkedList<Type>::Iterator cbegin() const
		{
			return Iterator(this);
		}

		const LinkedList<Type>::Iterator cend() const
		{
			return Iterator(nullptr);
		}

	public:
		// LinkedList<Type>*	Previous() const;
		LinkedList<Type>* Next() const;

		// LinkedList<Type>*	Front() const;
		LinkedList<Type>* Back() const;

		void InsertNext(LinkedList<Type>* next);

		void PushBack(LinkedList<Type>* element);

		Type* Data() const;

	private:
		LinkedList<Type>* _next = nullptr;
		Type*             _data = nullptr;
	};
}

#include "LinkedList.inl"
