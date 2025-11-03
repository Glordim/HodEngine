namespace hod
{
	/// @brief
	/// @tparam __TYPE__
	/// @param index
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::IteratorBase(pointer ptr)
	: _ptr(ptr)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::IteratorBase(const IteratorBase& iterator)
	: _ptr(iterator._ptr)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::IteratorBase(IteratorBase&& iterator)
	: _ptr(iterator._ptr)
	{
		iterator._ptr = nullptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>& Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator=(const IteratorBase& iterator)
	{
		_ptr = iterator._ptr;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>& Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator=(IteratorBase&& iterator)
	{
		_ptr = iterator._ptr;

		iterator._ptr = nullptr;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	bool Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator==(const IteratorBase& iterator) const
	{
		return _ptr == iterator._ptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	bool Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator!=(const IteratorBase& iterator) const
	{
		return _ptr != iterator._ptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>& Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator+=(difference_type offset)
	{
		_ptr += offset; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>& Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator-=(difference_type offset)
	{
		_ptr -= offset; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__> Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator+(difference_type offset) const
	{
		return typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>(_ptr + offset); // TODO : Check bound if macro defined
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__> Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator-(difference_type offset) const
	{
		return typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>(_ptr - offset); // TODO : Check bound if macro defined
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>& Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator++()
	{
		++_ptr; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>& Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator--()
	{
		--_ptr; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__> Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator++(int32_t)
	{
		IteratorBase it = IteratorBase(*this);
		++_ptr; // TODO : Check bound if macro defined

		return it;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__> Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator--(int32_t)
	{
		IteratorBase it = IteratorBase(*this);
		--_ptr; // TODO : Check bound if macro defined

		return it;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>::reference Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator*() const
	{
		return *_ptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>::pointer Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator->() const
	{
		return _ptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>::difference_type Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator-(const IteratorBase& iterator) const
	{
		return _ptr - iterator._ptr;
	}

	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	typename Vector<__TYPE__>::template IteratorBase<__ITER_TYPE__>::reference Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator[](difference_type offset) const
	{
		return *(_ptr + offset);
	}

	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	bool Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator<(const IteratorBase& iterator) const
	{
		return _ptr - iterator._ptr < 0;
	}

	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	bool Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator<=(const IteratorBase& iterator) const
	{
		return _ptr - iterator._ptr <= 0;
	}

	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	bool Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator>(const IteratorBase& iterator) const
	{
		return _ptr - iterator._ptr > 0;
	}

	template<typename __TYPE__>
	template<typename __ITER_TYPE__>
	bool Vector<__TYPE__>::IteratorBase<__ITER_TYPE__>::operator>=(const IteratorBase& iterator) const
	{
		return _ptr - iterator._ptr >= 0;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param allocator
	template<typename __TYPE__>
	Vector<__TYPE__>::Vector(Allocator& allocator)
	: _allocator(&allocator)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param count
	/// @param allocator
	template<typename __TYPE__>
	Vector<__TYPE__>::Vector(uint32_t count, Allocator& allocator)
	: _capacity(count)
	, _size(count)
	, _allocator(&allocator)
	{
		_elements = (_capacity != 0) ? reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__))) : nullptr;

		for (uint32_t index = 0; index < _size; ++index)
		{
			New(&_elements[index]);
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param values
	/// @param allocator
	template<typename __TYPE__>
	Vector<__TYPE__>::Vector(std::initializer_list<__TYPE__> values, Allocator& allocator)
	: _capacity(static_cast<uint32_t>(values.size()))
	, _size(static_cast<uint32_t>(values.size()))
	, _allocator(&allocator)
	{
		_elements = (_capacity != 0) ? reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__))) : nullptr;

		for (uint32_t index = 0; index < _size; ++index)
		{
			New(&_elements[index], values.begin()[index]);
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	/// @param allocator
	template<typename __TYPE__>
	Vector<__TYPE__>::Vector(const Vector<__TYPE__>& vector, Allocator& allocator)
	: _capacity(vector._size)
	, _size(vector._size)
	, _allocator(&allocator)
	{
		_elements = (_capacity != 0) ? reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__))) : nullptr;

		for (uint32_t index = 0; index < _size; ++index)
		{
			New(&_elements[index], vector._elements[index]);
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	template<typename __TYPE__>
	Vector<__TYPE__>::Vector(Vector<__TYPE__>&& vector)
	: _elements(vector._elements)
	, _capacity(vector._capacity)
	, _size(vector._size)
	, _allocator(vector._allocator)
	{
		vector._elements = nullptr;
		vector._capacity = 0;
		vector._size = 0;
		// Keep allocator pointer
	}

	/// @brief
	/// @tparam __TYPE__
	template<typename __TYPE__>
	Vector<__TYPE__>::~Vector()
	{
		FreeAndDestructElements();
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param values
	/// @return
	template<typename __TYPE__>
	Vector<__TYPE__>& Vector<__TYPE__>::operator=(std::initializer_list<__TYPE__> values)
	{
		Assign(values.begin(), values.size());
		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	/// @return
	template<typename __TYPE__>
	Vector<__TYPE__>& Vector<__TYPE__>::operator=(const Vector& vector)
	{
		if (&vector == this)
		{
			return *this;
		}

		Assign(vector._elements, vector._size);
		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	/// @return
	template<typename __TYPE__>
	Vector<__TYPE__>& Vector<__TYPE__>::operator=(Vector&& vector)
	{
		FreeAndDestructElements();

		_elements = vector._elements;
		_capacity = vector._capacity;
		_size = vector._size;
		_allocator = vector._allocator;

		vector._elements = nullptr;
		vector._capacity = 0;
		vector._size = 0;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::operator==(const Vector& vector) const
	{
		if (_size != vector._size)
		{
			return false;
		}

		for (uint32_t index = 0; index < _size; ++index)
		{
			if (_elements[index] != vector._elements[index])
			{
				return false;
			}
		}

		return true;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param position
	/// @return
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::operator[](uint32_t position) const&
	{
		return At(position);
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param position
	/// @return
	template<typename __TYPE__>
	__TYPE__& Vector<__TYPE__>::operator[](uint32_t position) &
	{
		return At(position);
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param position
	/// @return
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::At(uint32_t position) const&
	{
		assert(position < _size);

		return _elements[position];
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param position
	/// @return
	template<typename __TYPE__>
	__TYPE__& Vector<__TYPE__>::At(uint32_t position) &
	{
		assert(position < _size);

		return _elements[position];
	}

	/// @brief Returns a const reference to the first element in the container
	/// @tparam __TYPE__
	/// @return
	/// @remark Calling Front on an empty vector is undefined.
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::Front() const&
	{
		assert(_size > 0);

		return _elements[0];
	}

	/// @brief Returns a reference to the first element in the container
	/// @tparam __TYPE__
	/// @return
	/// @remark Calling Front on an empty vector is undefined.
	template<typename __TYPE__>
	__TYPE__& Vector<__TYPE__>::Front() &
	{
		assert(_size > 0);

		return _elements[0];
	}

	/// @brief Returns a const reference to the last element in the container
	/// @tparam __TYPE__
	/// @return
	/// @remark Calling Back on an empty vector is undefined.
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::Back() const&
	{
		assert(_size > 0);

		return _elements[_size - 1];
	}

	/// @brief Returns a reference to the last element in the container
	/// @tparam __TYPE__
	/// @return
	/// @remark Calling Back on an empty vector is undefined.
	template<typename __TYPE__>
	__TYPE__& Vector<__TYPE__>::Back() &
	{
		assert(_size > 0);

		return _elements[_size - 1];
	}

	/// @brief Returns an iterator to the first element of the vector.
	/// @tparam __TYPE__
	/// @return
	/// @remark If the vector is empty, the returned iterator will be equal to end()
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator Vector<__TYPE__>::CBegin() const&
	{
		return ConstIterator(const_cast<__TYPE__*>(_elements));
	}

	/// @brief Returns an iterator to the element following the last element of the vector.
	/// @tparam __TYPE__
	/// @return
	/// @remark Attempting to access it results in undefined behavior
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator Vector<__TYPE__>::CEnd() const&
	{
		return ConstIterator(_elements + _size);
	}

	/// @brief Returns an iterator to the first element of the vector.
	/// @tparam __TYPE__
	/// @return
	/// @remark If the vector is empty, the returned iterator will be equal to end()
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Begin() &
	{
		return Iterator(_elements);
	}

	/// @brief Returns an iterator to the element following the last element of the vector.
	/// @tparam __TYPE__
	/// @return
	/// @remark Attempting to access it results in undefined behavior
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::End() &
	{
		return Iterator(_elements + _size);
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstReverseIterator Vector<__TYPE__>::CRBegin() const&
	{
		return ConstReverseIterator(CEnd());
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstReverseIterator Vector<__TYPE__>::CREnd() const&
	{
		return ConstReverseIterator(CBegin());
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::RBegin() &
	{
		return ReverseIterator(End());
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::REnd() &
	{
		return ReverseIterator(Begin());
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	__TYPE__* Vector<__TYPE__>::begin() &
	{
		return _elements;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	const __TYPE__* Vector<__TYPE__>::begin() const&
	{
		return _elements;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	__TYPE__* Vector<__TYPE__>::end() &
	{
		return _elements + _size;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	const __TYPE__* Vector<__TYPE__>::end() const&
	{
		return _elements + _size;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	const __TYPE__* Vector<__TYPE__>::Data() const&
	{
		return _elements;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	__TYPE__* Vector<__TYPE__>::Data() &
	{
		return _elements;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Capacity() const
	{
		return _capacity;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Size() const
	{
		return _size;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::Empty() const
	{
		return _size == 0;
	}

	/// @brief Increase the capacity of the vector to a value that's greater or equal to the new capacity. If the new capacity is greater than the current capacity, new storage is
	/// allocated, otherwise the method does nothing
	/// @tparam __TYPE__
	/// @param capacity
	template<typename __TYPE__>
	void Vector<__TYPE__>::Reserve(uint32_t capacity)
	{
		if (capacity > _capacity)
		{
			if (_elements == nullptr)
			{
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(capacity * sizeof(__TYPE__), alignof(__TYPE__)));
				_capacity = capacity;
			}
			else
			{
				Reallocate(capacity);
			}
		}
	}

	/// @brief Erases all elements from the container. After this call, size() returns zero.
	/// @tparam __TYPE__
	/// @remark	Leaves the capacity() of the vector unchanged
	template<typename __TYPE__>
	void Vector<__TYPE__>::Clear()
	{
		for (uint32_t index = 0; index < _size; ++index)
		{
			_elements[index].~__TYPE__();
		}

		_size = 0;
	}

	/// @brief Requests the container to reduce its capacity to fit its size
	/// @tparam __TYPE__
	template<typename __TYPE__>
	void Vector<__TYPE__>::ShrinkToFit()
	{
		if (_size == 0)
		{
			_allocator->Free(_elements);
			_elements = nullptr;
			_capacity = 0;
		}
		else if (_capacity > _size)
		{
			Reallocate(_size);
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param size
	template<typename __TYPE__>
	void Vector<__TYPE__>::Resize(uint32_t newSize)
	{
		if (newSize == _size)
		{
			return;
		}

		if (newSize > _size)
		{
			if (_capacity < newSize)
			{
				if (_elements == nullptr)
				{
					_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(newSize * sizeof(__TYPE__), alignof(__TYPE__)));
					_capacity = newSize;
				}
				else
				{
					Reallocate(newSize);
				}
			}
			for (uint32_t index = _size; index < newSize; ++index)
			{
				New(&_elements[index]);
			}
			_size = newSize;
		}
		else
		{
			if (_size > 0)
			{
				for (uint32_t index = _size - 1; index > newSize; --index) // > to avoid infinite loop if _size is 1 and size is 0
				{
					_elements[index].~__TYPE__();
				}

				_elements[newSize].~__TYPE__();
			}

			_size = newSize;
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param size
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::Resize(uint32_t newSize, const __TYPE__& value)
	{
		if (newSize == _size)
		{
			return;
		}

		if (newSize > _size)
		{
			if (_capacity < newSize)
			{
				if (_elements == nullptr)
				{
					_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(newSize * sizeof(__TYPE__), alignof(__TYPE__)));
					_capacity = newSize;
				}
				else
				{
					Reallocate(newSize);
				}
			}
			for (uint32_t index = _size; index < newSize; ++index)
			{
				New(&_elements[index], value);
			}
			_size = newSize;
		}
		else
		{
			if (_size > 0)
			{
				for (uint32_t index = _size - 1; index > newSize; --index) // > to avoid infinite loop if _size is 1 and size is 0
				{
					_elements[index].~__TYPE__();
				}

				_elements[newSize].~__TYPE__();
			}

			_size = newSize;
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::PushBack(const __TYPE__& value)
	{
		if (_size >= _capacity)
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
			}
			else
			{
				Reallocate(_capacity * 2);
			}
		}

		New(&_elements[_size], value);
		++_size;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::PushBack(__TYPE__&& value)
	{
		if (_size >= _capacity)
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
			}
			else
			{
				Reallocate(_capacity * 2);
			}
		}

		New(&_elements[_size], std::move(value));
		++_size;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::push_back(const __TYPE__& value)
	{
		PushBack(value);
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::push_back(__TYPE__&& value)
	{
		PushBack(std::move(value));
	}

	/// @brief Removes the last element of the container. This destroys the removed element
	/// @tparam __TYPE__
	/// @remark Calling PopBack on an empty container is undefined
	template<typename __TYPE__>
	void Vector<__TYPE__>::PopBack()
	{
		assert(_size > 0);

		// std::is_destructible should be called but is it returns false it means that the destructor is deleted and thus it generated a compilation error...
		_elements[_size - 1].~__TYPE__();
		--_size;
	}

	/// @brief Inserts value at iterator position. Reverse iterators are used as forward iterators, so the element will be inserted after the iterator in this case
	/// @tparam __TYPE__
	/// @param iterator
	/// @param value
	/// @return
	/// @remark If the vector size is 0, then iterator must be equal to begin
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Insert(ConstIterator iterator, const __TYPE__& value)
	{
		uint32_t index = iterator._ptr - _elements;
		return Begin() + Insert(index, value);
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Insert(uint32_t index, const __TYPE__& value)
	{
		assert(index <= _size);

		if (_size >= _capacity)
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
			}
			else
			{
				ReallocateAndMove(_capacity * 2, index, 1);
			}
		}
		else
		{
			Move(index, 1);
		}

		New(&_elements[index], value);
		++_size;

		return index;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @param value
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::Insert(ConstReverseIterator iterator, const __TYPE__& value)
	{
		return ReverseIterator(Insert(--iterator.base(), value));
	}

	/// @brief Inserts value at iterator position. Reverse iterators are used as forward iterators, so the element will be inserted after the iterator in this case
	/// @tparam __TYPE__
	/// @param iterator
	/// @param value
	/// @return
	/// @remark If the vector size is 0, then iterator must be equal to begin
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Insert(ConstIterator iterator, __TYPE__&& value)
	{
		uint32_t index = iterator._ptr - _elements;
		return Begin() + Insert(index, std::move(value));
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Insert(uint32_t index, __TYPE__&& value)
	{
		assert(index <= _size);

		if (_size >= _capacity)
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
			}
			else
			{
				ReallocateAndMove(_capacity * 2, index, 1);
			}
		}
		else
		{
			Move(index, 1);
		}

		New(&_elements[index], std::move(value));
		++_size;

		return index;
	}

	template<typename __TYPE__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::Insert(ConstReverseIterator iterator, __TYPE__&& value)
	{
		return ReverseIterator(Insert(--iterator.base(), std::move(value)));
	}

	/// @brief Inserts value at iterator position if copy constructor exists
	/// @tparam __TYPE__
	/// @param iterator
	/// @param value
	/// @return
	/// @remark If the vector size is 0, then iterator must be equal to begin
	template<typename __TYPE__>
	template<typename __ITERATOR__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Insert(ConstIterator iterator, __ITERATOR__ first, __ITERATOR__ last)
	{
		static_assert(!(std::is_same_v<__ITERATOR__, ReverseIterator> || std::is_same_v<__ITERATOR__, ConstReverseIterator>),
		              "Insert Range with reverse iterators is not supported");

		uint32_t index = iterator._ptr - _elements;
		return Begin() + Insert(index, first, last);
	}

	template<typename __TYPE__>
	template<typename __ITERATOR__>
	uint32_t Vector<__TYPE__>::Insert(uint32_t index, __ITERATOR__ first, __ITERATOR__ last)
	{
		assert(index <= _size);

		uint32_t elementToInsertCount = std::distance(first, last);
		if (elementToInsertCount == 0)
		{
			return index;
		}

		if (_size + elementToInsertCount > _capacity)
		{
			if (_elements == nullptr)
			{
				_capacity = elementToInsertCount;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
			}
			else
			{
				uint32_t newCapacity = _capacity * 2;
				while (newCapacity < _size + elementToInsertCount)
				{
					newCapacity *= 2;
				}
				ReallocateAndMove(newCapacity, index, elementToInsertCount);
			}
		}
		else
		{
			Move(index, elementToInsertCount);
		}

		if constexpr (std::is_trivially_copyable_v<__TYPE__> && std::contiguous_iterator<__ITERATOR__>)
		{
			std::memcpy(_elements + index, std::to_address(first), elementToInsertCount * sizeof(__TYPE__));
		}
		else
		{
			__ITERATOR__ it = first;
			for (uint32_t i = 0; i < elementToInsertCount; ++i, ++it)
			{
				New(&_elements[index + i], __TYPE__(*it));
			}
		}
		_size += elementToInsertCount;

		return index + elementToInsertCount;
	}

	template<typename __TYPE__>
	template<typename __ITERATOR__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::Insert(ConstReverseIterator iterator, __ITERATOR__ first, __ITERATOR__ last)
	{
		return ReverseIterator(Insert(--iterator.base(), first, last));
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param ...arguments
	template<typename __TYPE__>
	template<typename... __ARGUMENTS__>
	__TYPE__& Vector<__TYPE__>::EmplaceBack(__ARGUMENTS__&&... arguments)
	{
		if (_size < _capacity)
		{
			New(&_elements[_size], std::forward<__ARGUMENTS__>(arguments)...);
			++_size;
		}
		else
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				New(&_elements[_size], std::forward<__ARGUMENTS__>(arguments)...);
				++_size;
			}
			else
			{
				Reallocate(_capacity * 2);

				New(&_elements[_size], std::forward<__ARGUMENTS__>(arguments)...);
				++_size;
			}
		}

		return _elements[_size - 1];
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @param ...arguments
	/// @return
	/// @remark If the vector size is 0, then iterator must be equal to begin
	template<typename __TYPE__>
	template<typename... __ARGUMENTS__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Emplace(ConstIterator iterator, __ARGUMENTS__&&... arguments)
	{
		if (_size < _capacity)
		{
			assert(iterator._ptr <= _elements + _size);

			if (iterator._ptr < _elements + _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = &_elements[_size - 1]; element > iterator._ptr; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + (iterator._ptr - _elements))->~__TYPE__();
			}

			New(_elements + (iterator._ptr - _elements), std::forward<__ARGUMENTS__>(arguments)...);
			++_size;

			return Iterator(const_cast<typename Iterator::pointer>(iterator._ptr));
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(iterator._ptr == nullptr);

				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				New(&_elements[_size], std::forward<__ARGUMENTS__>(arguments)...);
				++_size;

				return Iterator(_elements);
			}
			else
			{
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					assert(iterator._ptr <= _elements + _size);

					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));

					for (__TYPE__* element = _elements; element < iterator._ptr; ++element)
					{
						New(elements, std::move(*element));
						++elements;
						element->~__TYPE__();
					}

					__TYPE__* newElement = elements;
					New(elements, std::forward<__ARGUMENTS__>(arguments)...);
					++elements;

					for (__TYPE__* element = _elements + (iterator._ptr - _elements); element < _elements + _size; ++element)
					{
						New(elements, std::move(*element));
						++elements;
						element->~__TYPE__();
					}

					++_size;

					_allocator->Free(_elements);

					_elements = elements - _size;
					_capacity = _capacity * 2;

					return Iterator(newElement);
				}
				else
				{
					for (__TYPE__* element = _elements + _size; element > iterator._ptr; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = const_cast<__TYPE__*>(iterator._ptr);
					New(newElement, std::forward<__ARGUMENTS__>(arguments)...);

					++_size;
					_capacity = _capacity * 2;

					return Iterator(newElement);
				}
			}
		}
	}

	template<typename __TYPE__>
	template<typename... __ARGUMENTS__>
	uint32_t Vector<__TYPE__>::Emplace(uint32_t index, __ARGUMENTS__&&... arguments)
	{
		if (_size < _capacity)
		{
			assert(index <= _size);

			if (index < _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = &_elements[_size - 1]; element > _elements + index; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + index)->~__TYPE__();
			}

			New(_elements + index, std::forward<__ARGUMENTS__>(arguments)...);
			++_size;

			return index;
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(index == 0);

				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				New(&_elements[_size], std::forward<__ARGUMENTS__>(arguments)...);
				++_size;

				return 0;
			}
			else
			{
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					assert(index <= _size);

					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));

					for (__TYPE__* element = _elements; element < _elements + index; ++element)
					{
						New(elements, std::move(*element));
						++elements;
						element->~__TYPE__();
					}

					__TYPE__* newElement = elements;
					New(elements, std::forward<__ARGUMENTS__>(arguments)...);
					++elements;

					for (__TYPE__* element = _elements + index; element < _elements + _size; ++element)
					{
						New(elements, std::move(*element));
						++elements;
						element->~__TYPE__();
					}

					++_size;

					_allocator->Free(_elements);

					_elements = elements - _size;
					_capacity = _capacity * 2;

					return static_cast<uint32_t>(newElement - _elements);
				}
				else
				{
					for (__TYPE__* element = _elements + _size; element > _elements + index; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = _elements + index;
					New(newElement, std::forward<__ARGUMENTS__>(arguments)...);

					++_size;
					_capacity = _capacity * 2;

					return static_cast<uint32_t>(newElement - _elements);
				}
			}
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @param ...arguments
	/// @return
	template<typename __TYPE__>
	template<typename... __ARGUMENTS__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::Emplace(ConstReverseIterator iterator, __ARGUMENTS__&&... arguments)
	{
		return ReverseIterator(Emplace(--iterator.base(), arguments...));
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param  ptr
	/// @param ...arguments
	template<typename __TYPE__>
	template<typename... __ARGUMENTS__>
	void Vector<__TYPE__>::New(void* ptr, __ARGUMENTS__&&... arguments)
	{
		if constexpr (std::is_constructible_v<__TYPE__, __ARGUMENTS__..., Allocator*> &&
		              !std::is_constructible_v<__TYPE__, __ARGUMENTS__..., void*>) // Clang will cast void* as Allocator*, making the first condition true for Vector<void*>
		{
			new (ptr) __TYPE__(std::forward<__ARGUMENTS__>(arguments)..., _allocator);
		}
		else
		{
			new (ptr) __TYPE__(std::forward<__ARGUMENTS__>(arguments)...);
		}
	}

	/// @brief Erases the specified elements from the container
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	/// @remark Can only be called if vector type is move assignable
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Erase(ConstIterator iterator)
	{
		assert(iterator._ptr < _elements + _size);

		for (__TYPE__* element = _elements + (iterator._ptr - _elements); element < _elements + _size - 1; ++element)
		{
			*element = std::move(*(element + 1));
		}

		_elements[_size - 1].~__TYPE__();
		--_size;

		return Iterator(const_cast<typename Iterator::pointer>(iterator._ptr));
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Erase(uint32_t index)
	{
		assert(index < _size);

		for (__TYPE__* element = _elements + index; element < _elements + _size - 1; ++element)
		{
			*element = std::move(*(element + 1));
		}

		_elements[_size - 1].~__TYPE__();
		--_size;

		return index;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::Erase(ConstReverseIterator iterator)
	{
		return ReverseIterator(Erase(--iterator.base()));
	}

	/// @brief Erases the elements in range from first to last excluded
	/// @tparam __TYPE__
	/// @param first
	/// @param last : Must be greater or equal than first
	/// @return Iterator following the last removed element
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Erase(ConstIterator first, ConstIterator last)
	{
		assert(first._ptr <= _elements + _size);
		assert(first._ptr <= last._ptr);

		if (first._ptr == last._ptr)
		{
			return Iterator(const_cast<typename Iterator::pointer>(last._ptr));
		}

		uint32_t  newSize = _size - static_cast<uint32_t>(last._ptr - first._ptr);
		__TYPE__* firstPtr = _elements + (first._ptr - _elements);

		for (__TYPE__* element = _elements + (last._ptr - _elements); element < _elements + _size; ++element)
		{
			*(firstPtr) = std::move(*element);
			++firstPtr;
		}

		for (__TYPE__* element = firstPtr; element < _elements + _size; ++element)
		{
			element->~__TYPE__();
		}

		_size = newSize;

		return Iterator(const_cast<typename Iterator::pointer>(first._ptr));
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Erase(uint32_t index, uint32_t count)
	{
		assert(index <= _size);

		if (!count)
		{
			return index;
		}

		uint32_t  newSize = _size - count;
		__TYPE__* firstPtr = _elements + index;

		for (__TYPE__* element = _elements + index + count; element < _elements + _size; ++element)
		{
			*(firstPtr) = std::move(*element);
			++firstPtr;
		}

		for (__TYPE__* element = firstPtr; element < _elements + _size; ++element)
		{
			element->~__TYPE__();
		}

		_size = newSize;

		return index;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param first
	/// @param last
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ReverseIterator Vector<__TYPE__>::Erase(ConstReverseIterator first, ConstReverseIterator last)
	{
		return ReverseIterator(Erase(--first.base(), --last.base()));
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	template<typename __TYPE__>
	void Vector<__TYPE__>::Swap(Vector& vector)
	{
		__TYPE__*  elements = _elements;
		uint32_t   capacity = _capacity;
		uint32_t   size = _size;
		Allocator* allocator = _allocator;

		_elements = vector._elements;
		_capacity = vector._capacity;
		_size = vector._size;
		_allocator = vector._allocator;

		vector._elements = elements;
		vector._capacity = capacity;
		vector._size = size;
		vector._allocator = allocator;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param itFirst
	/// @param itSecond
	template<typename __TYPE__>
	void Vector<__TYPE__>::Swap(ConstIterator itFirst, ConstIterator itSecond)
	{
		assert((itFirst._ptr < _elements + _size) && (itSecond._ptr < _elements + _size));

		__TYPE__ value(std::move(const_cast<__TYPE__&>(*itFirst._ptr)));
		*(_elements + (itFirst._ptr - _elements)) = std::move(*const_cast<__TYPE__*>(itSecond._ptr));
		*(_elements + (itSecond._ptr - _elements)) = std::move(value);
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::Swap(uint32_t first, uint32_t second)
	{
		assert((first < _size) && (second < _size));

		__TYPE__ value(std::move(const_cast<__TYPE__&>(*(_elements + first))));
		*(_elements + first) = std::move(*const_cast<__TYPE__*>(_elements + second));
		*(_elements + second) = std::move(value);
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::Swap(ConstReverseIterator itFirst, ConstReverseIterator itSecond)
	{
		Swap(--itFirst.base(), --itSecond.base());
	}

	/// @brief Swap the element at iterator position with the back element and then pop back
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	void Vector<__TYPE__>::SwapAndPopBack(ConstIterator iterator)
	{
		assert(iterator._ptr < _elements + _size);

		if (iterator._ptr < _elements + _size - 1)
		{
			Swap(iterator, ConstIterator(_elements + _size - 1));
		}

		if (iterator._ptr < _elements + _size)
		{
			PopBack();
		}
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::SwapAndPopBack(uint32_t index)
	{
		assert(index < _size);

		if (index < _size - 1)
		{
			Swap(index, _size - 1);
		}

		if (index < _size)
		{
			PopBack();
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	void Vector<__TYPE__>::SwapAndPopBack(ConstReverseIterator iterator)
	{
		SwapAndPopBack(--iterator.base());
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::Assign(const __TYPE__* source, uint32_t count)
	{
		if (_capacity >= count)
		{
			uint32_t minSize = (_size < count) ? _size : count;

			for (uint32_t i = 0; i < minSize; ++i)
			{
				_elements[i] = source[i];
			}

			for (uint32_t i = minSize; i < count; ++i)
			{
				New(&_elements[i], source[i]);
			}

			for (uint32_t i = count; i < _size; ++i)
			{
				_elements[i].~__TYPE__();
			}

			_size = count;
		}
		else
		{
			FreeAndDestructElements();

			_capacity = count;
			_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

			for (uint32_t i = 0; i < count; ++i)
			{
				New(&_elements[i], source[i]);
			}

			_size = count;
		}
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::Reallocate(uint32_t capacity)
	{
		if (_allocator->Resize(_elements, capacity * sizeof(__TYPE__), alignof(__TYPE__)) == false)
		{
			__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(capacity * sizeof(__TYPE__), alignof(__TYPE__)));

			if constexpr (std::is_trivially_copyable_v<__TYPE__>)
			{
				std::memcpy(elements, _elements, _size * sizeof(__TYPE__));
			}
			else
			{
				for (uint32_t index = 0; index < _size; ++index)
				{
					New(&elements[index], std::move(_elements[index]));
					_elements[index].~__TYPE__();
				}
			}

			_allocator->Free(_elements);
			_elements = elements;
		}
		_capacity = capacity;
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::ReallocateAndMove(uint32_t capacity, uint32_t moveIndex, uint32_t moveLen)
	{
		if (_allocator->Resize(_elements, capacity * sizeof(__TYPE__), alignof(__TYPE__)) == false)
		{
			__TYPE__* elements = static_cast<__TYPE__*>(_allocator->Allocate(capacity * sizeof(__TYPE__), alignof(__TYPE__)));

			if constexpr (std::is_trivially_copyable_v<__TYPE__>)
			{
				std::memcpy(elements, _elements, moveIndex * sizeof(__TYPE__));
				std::memcpy(elements + moveIndex + moveLen, _elements + moveIndex, (_size - moveIndex) * sizeof(__TYPE__));
			}
			else
			{
				for (uint32_t index = 0; index < moveIndex; ++index)
				{
					New(&elements[index], std::move(_elements[index]));
					_elements[index].~__TYPE__();
				}

				for (uint32_t index = moveIndex; index < _size; ++index)
				{
					New(&elements[index + moveLen], std::move(_elements[index]));
					_elements[index].~__TYPE__();
				}
			}
			_allocator->Free(_elements);
			_elements = elements;
		}
		else
		{
			Move(moveIndex, moveLen);
		}
		_capacity = capacity;
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::Move(uint32_t moveIndex, uint32_t moveLen)
	{
		if constexpr (std::is_trivially_copyable_v<__TYPE__>)
		{
			std::memmove(_elements + moveIndex + moveLen, _elements + moveIndex, (_size - moveIndex) * sizeof(__TYPE__));
		}
		else
		{
			for (int32_t i = moveLen - 1; i >= 0; --i)
			{
				New(&_elements[moveIndex + moveLen + i], std::move(_elements[moveIndex + i]));
				_elements[moveIndex + i].~__TYPE__();
			}
		}
	}

	template<typename __TYPE__>
	void Vector<__TYPE__>::FreeAndDestructElements()
	{
		if (_elements != nullptr)
		{
			for (uint32_t index = 0; index < _size; ++index)
			{
				_elements[index].~__TYPE__();
			}

			_allocator->Free(_elements);
		}
	}
}
