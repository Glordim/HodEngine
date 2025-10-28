#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/Allocator.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

namespace hod
{
	template<typename __TYPE__>
	class Vector
	{
	public:
		using value_type = __TYPE__;

	public:
		template<typename __ITER_TYPE__>
		class IteratorBase
		{
			friend class Vector;

		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = __ITER_TYPE__;
			using pointer = __ITER_TYPE__*;
			using reference = __ITER_TYPE__&;

		public:
			IteratorBase() = default;
			IteratorBase(const IteratorBase& iterator);
			IteratorBase(IteratorBase&& iterator);
			~IteratorBase() = default;

			IteratorBase& operator=(const IteratorBase& iterator);
			IteratorBase& operator=(IteratorBase&& iterator);

			template<typename OtherValueType>
				requires(std::is_same_v<std::remove_const_t<__ITER_TYPE__>, std::remove_const_t<OtherValueType>> && std::is_const_v<__ITER_TYPE__> &&
			             !std::is_const_v<OtherValueType>)
			IteratorBase(const IteratorBase<OtherValueType>& other)
			: _ptr(other._ptr)
			{
			}

			bool operator==(const IteratorBase& iterator) const;
			bool operator!=(const IteratorBase& iterator) const;

			IteratorBase& operator+=(difference_type offset);
			IteratorBase& operator-=(difference_type offset);

			IteratorBase operator+(difference_type offset) const;
			IteratorBase operator-(difference_type offset) const;

			IteratorBase& operator++();
			IteratorBase& operator--();

			IteratorBase operator++(int32_t);
			IteratorBase operator--(int32_t);

			reference operator*() const;
			pointer   operator->() const;

			difference_type operator-(const IteratorBase& iterator) const;

			reference operator[](difference_type offset) const;

			bool operator<(const IteratorBase& iterator) const;
			bool operator<=(const IteratorBase& iterator) const;
			bool operator>(const IteratorBase& iterator) const;
			bool operator>=(const IteratorBase& iterator) const;

		private:
			IteratorBase(pointer ptr);

		private:
			pointer _ptr = nullptr;
		};

		using Iterator = IteratorBase<__TYPE__>;
		using ConstIterator = IteratorBase<const __TYPE__>;
		using ReverseIterator = std::reverse_iterator<Iterator>;
		using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

	public:
		Vector() = default;
		explicit Vector(Allocator& allocator); // Not using default value and make it explicit to avoid some mismatch with std::is_constructible_v (See New<>())
		explicit Vector(uint32_t count, Allocator& allocator = DefaultAllocator::GetInstance());
		Vector(std::initializer_list<__TYPE__> values, Allocator& allocator = DefaultAllocator::GetInstance());
		Vector(const Vector& vector, Allocator& allocator = DefaultAllocator::GetInstance());
		Vector(Vector&& vector);
		Vector(Vector&&, Allocator*) = delete; // Avoid compilation errors (See New<>())
		~Vector();

		Vector& operator=(std::initializer_list<__TYPE__> values);
		Vector& operator=(const Vector& vector);
		Vector& operator=(Vector&& vector);

		bool operator==(const Vector& vector) const;
		bool operator!=(const Vector& vector) const;

		const __TYPE__& operator[](uint32_t index) const&;
		__TYPE__&       operator[](uint32_t index) &;

		const __TYPE__& At(uint32_t index) const&;
		__TYPE__&       At(uint32_t index) &;

		const __TYPE__& Front() const&;
		__TYPE__&       Front() &;
		const __TYPE__& Back() const&;
		__TYPE__&       Back() &;

		ConstIterator CBegin() const&;
		ConstIterator CEnd() const&;
		Iterator      Begin() &;
		Iterator      End() &;

		ConstReverseIterator CRBegin() const&;
		ConstReverseIterator CREnd() const&;
		ReverseIterator      RBegin() &;
		ReverseIterator      REnd() &;

		__TYPE__*       begin() &;      // Only for compatibility with C++11's range-based for loops.
		const __TYPE__* begin() const&; // Only for compatibility with C++11's range-based for loops.
		__TYPE__*       end() &;        // Only for compatibility with C++11's range-based for loops.
		const __TYPE__* end() const&;   // Only for compatibility with C++11's range-based for loops.

		const __TYPE__* Data() const&;
		__TYPE__*       Data() &;

		uint32_t Capacity() const;
		uint32_t Size() const;
		bool     Empty() const;

		void Reserve(uint32_t capacity);
		void Clear();
		void ShrinkToFit();
		void Resize(uint32_t size);
		void Resize(uint32_t size, const __TYPE__& value);

		// TODO : void			Assign(uint32_t size, const __TYPE__& value);

		void PushBack(const __TYPE__& value);
		void PushBack(__TYPE__&& value);
		void push_back(const __TYPE__& value); // Only for compatibility with back_inserter
		void push_back(__TYPE__&& value);      // Only for compatibility with back_inserter
		void PopBack();

		Iterator        Insert(ConstIterator iterator, const __TYPE__& value);
		ReverseIterator Insert(ConstReverseIterator iterator, const __TYPE__& value);
		Iterator        Insert(ConstIterator iterator, __TYPE__&& value);
		ReverseIterator Insert(ConstReverseIterator iterator, __TYPE__&& value);
		template<typename __Iterator__>
		Iterator Insert(ConstIterator iterator, __Iterator__ first, __Iterator__ last);
		template<typename __Iterator__>
		ReverseIterator Insert(ConstReverseIterator iterator, __Iterator__ first, __Iterator__ last);
		uint32_t        Insert(uint32_t index, const __TYPE__& value);
		uint32_t        Insert(uint32_t index, __TYPE__&& value);
		template<typename __Iterator__>
		uint32_t Insert(uint32_t index, __Iterator__ first, __Iterator__ last);

		template<typename... __ARGUMENTS__>
		__TYPE__& EmplaceBack(__ARGUMENTS__&&... arguments);
		template<typename... __ARGUMENTS__>
		Iterator Emplace(ConstIterator iterator, __ARGUMENTS__&&... arguments);
		template<typename... __ARGUMENTS__>
		ReverseIterator Emplace(ConstReverseIterator iterator, __ARGUMENTS__&&... arguments);
		template<typename... __ARGUMENTS__>
		uint32_t Emplace(uint32_t index, __ARGUMENTS__&&... arguments);

		Iterator        Erase(ConstIterator iterator);
		ReverseIterator Erase(ConstReverseIterator iterator);
		Iterator        Erase(ConstIterator first, ConstIterator last);
		ReverseIterator Erase(ConstReverseIterator first, ConstReverseIterator last);
		uint32_t        Erase(uint32_t index);
		uint32_t        Erase(uint32_t index, uint32_t count);

		void Swap(Vector& vector);
		void Swap(ConstIterator itFirst, ConstIterator itSecond);
		void Swap(ConstReverseIterator itFirst, ConstReverseIterator itSecond);
		void Swap(uint32_t first, uint32_t second);

		void SwapAndPopBack(ConstIterator iterator);
		void SwapAndPopBack(ConstReverseIterator iterator);
		void SwapAndPopBack(uint32_t index);

		// Deleted functions to prevent access to raw buffer on temporary (rvalue) vector
		// Needed because C++ can convert implicitly rvalue to const lvalue
		const __TYPE__&      operator[](uint32_t index) const&& = delete;
		const __TYPE__&      At(uint32_t index) const&& = delete;
		const __TYPE__&      Front() const&& = delete;
		const __TYPE__&      Back() const&& = delete;
		ConstIterator        CBegin() const&& = delete;
		ConstIterator        CEnd() const&& = delete;
		ConstReverseIterator CRBegin() const&& = delete;
		ConstReverseIterator CREnd() const&& = delete;
		const __TYPE__*      begin() const&& = delete;
		const __TYPE__*      end() const&& = delete;
		const __TYPE__*      Data() const&& = delete;

	private:
		template<typename... __ARGUMENTS__>
		void New(void* ptr, __ARGUMENTS__&&... arguments);

	private:
		__TYPE__* _elements {nullptr};
		uint32_t  _capacity {0};
		uint32_t  _size {0};

		Allocator* _allocator {&DefaultAllocator::GetInstance()};
	};
}

#include "Vector.tpp"
