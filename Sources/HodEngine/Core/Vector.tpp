namespace hod
{
	/// @brief
	/// @tparam __TYPE__
	/// @param index
	template<typename __TYPE__>
	Vector<__TYPE__>::Iterator::Iterator(pointer element)
	: element(element)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	Vector<__TYPE__>::Iterator::Iterator(const Iterator& iterator)
	: element(iterator.element)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	Vector<__TYPE__>::Iterator::Iterator(Iterator&& iterator)
	: element(iterator.element)
	{
		iterator.element = nullptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator& Vector<__TYPE__>::Iterator::operator=(const Iterator& iterator)
	{
		element = iterator.element;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator& Vector<__TYPE__>::Iterator::operator=(Iterator&& iterator)
	{
		element = iterator.element;

		iterator.element = nullptr;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::Iterator::operator==(const Iterator& iterator) const
	{
		return element == iterator.element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::Iterator::operator!=(const Iterator& iterator) const
	{
		return element != iterator.element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator& Vector<__TYPE__>::Iterator::operator+=(difference_type offset)
	{
		element += offset; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator& Vector<__TYPE__>::Iterator::operator-=(difference_type offset)
	{
		element -= offset; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Iterator::operator+(difference_type offset) const
	{
		return Vector<__TYPE__>::Iterator(element + offset); // TODO : Check bound if macro defined
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Iterator::operator-(difference_type offset) const
	{
		return Vector<__TYPE__>::Iterator(element - offset); // TODO : Check bound if macro defined
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator& Vector<__TYPE__>::Iterator::operator++()
	{
		++element; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator& Vector<__TYPE__>::Iterator::operator--()
	{
		--element; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Iterator::operator++(int32_t)
	{
		Iterator it = Iterator(*this);
		++element; // TODO : Check bound if macro defined

		return it;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator Vector<__TYPE__>::Iterator::operator--(int32_t)
	{
		Iterator it = Iterator(*this);
		--element; // TODO : Check bound if macro defined

		return it;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	__TYPE__& Vector<__TYPE__>::Iterator::operator*() const
	{
		return *element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	__TYPE__* Vector<__TYPE__>::Iterator::operator->() const
	{
		return element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator::difference_type Vector<__TYPE__>::Iterator::operator-(const Iterator& iterator) const
	{
		return element - iterator.element;
	}

	template<typename __TYPE__>
	typename Vector<__TYPE__>::Iterator::reference Vector<__TYPE__>::Iterator::operator[](difference_type offset) const
	{
		return *(element + offset);
	}

	template<typename __TYPE__>
	bool Vector<__TYPE__>::Iterator::operator<(const Iterator& iterator) const
	{
		return element - iterator.element < 0;
	}

	template<typename __TYPE__>
	bool Vector<__TYPE__>::Iterator::operator<=(const Iterator& iterator) const
	{
		return element - iterator.element <= 0;
	}

	template<typename __TYPE__>
	bool Vector<__TYPE__>::Iterator::operator>(const Iterator& iterator) const
	{
		return element - iterator.element > 0;
	}

	template<typename __TYPE__>
	bool Vector<__TYPE__>::Iterator::operator>=(const Iterator& iterator) const
	{
		return element - iterator.element >= 0;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param index
	template<typename __TYPE__>
	Vector<__TYPE__>::ConstIterator::ConstIterator(pointer element)
	: element(element)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	Vector<__TYPE__>::ConstIterator::ConstIterator(const ConstIterator& iterator)
	: element(iterator.element)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	Vector<__TYPE__>::ConstIterator::ConstIterator(const Iterator& iterator)
	: element(iterator.element)
	{
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	Vector<__TYPE__>::ConstIterator::ConstIterator(ConstIterator&& iterator)
	: element(iterator.element)
	{
		iterator.element = nullptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	template<typename __TYPE__>
	Vector<__TYPE__>::ConstIterator::ConstIterator(Iterator&& iterator)
	: element(iterator.element)
	{
		iterator.element = nullptr;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator& Vector<__TYPE__>::ConstIterator::operator=(const ConstIterator& iterator)
	{
		element = iterator.element;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator& Vector<__TYPE__>::ConstIterator::operator=(ConstIterator&& iterator)
	{
		element = iterator.element;

		iterator.element = nullptr;

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::ConstIterator::operator==(const ConstIterator& iterator) const
	{
		return element == iterator.element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::ConstIterator::operator!=(const ConstIterator& iterator) const
	{
		return element != iterator.element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator& Vector<__TYPE__>::ConstIterator::operator+=(difference_type offset)
	{
		element += offset; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator& Vector<__TYPE__>::ConstIterator::operator-=(difference_type offset)
	{
		element -= offset; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator Vector<__TYPE__>::ConstIterator::operator+(difference_type offset) const
	{
		return Vector<__TYPE__>::ConstIterator(element + offset); // TODO : Check bound if macro defined
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator Vector<__TYPE__>::ConstIterator::operator-(difference_type offset) const
	{
		return Vector<__TYPE__>::ConstIterator(element - offset); // TODO : Check bound if macro defined
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator& Vector<__TYPE__>::ConstIterator::operator++()
	{
		++element; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator& Vector<__TYPE__>::ConstIterator::operator--()
	{
		--element; // TODO : Check bound if macro defined

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator Vector<__TYPE__>::ConstIterator::operator++(int32_t)
	{
		ConstIterator it = ConstIterator(*this);
		++element; // TODO : Check bound if macro defined

		return it;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator Vector<__TYPE__>::ConstIterator::operator--(int32_t)
	{
		ConstIterator it = ConstIterator(*this);
		--element; // TODO : Check bound if macro defined

		return it;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::ConstIterator::operator*() const
	{
		return *element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @return
	template<typename __TYPE__>
	const __TYPE__* Vector<__TYPE__>::ConstIterator::operator->() const
	{
		return element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator::difference_type Vector<__TYPE__>::ConstIterator::operator-(const ConstIterator& iterator) const
	{
		return element - iterator.element;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param offset
	/// @return
	template<typename __TYPE__>
	typename Vector<__TYPE__>::ConstIterator::reference Vector<__TYPE__>::ConstIterator::operator[](difference_type offset) const
	{
		return *(element + offset);
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::ConstIterator::operator<(const ConstIterator& iterator) const
	{
		return element - iterator.element < 0;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::ConstIterator::operator<=(const ConstIterator& iterator) const
	{
		return element - iterator.element <= 0;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::ConstIterator::operator>(const ConstIterator& iterator) const
	{
		return element - iterator.element > 0;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param iterator
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::ConstIterator::operator>=(const ConstIterator& iterator) const
	{
		return element - iterator.element >= 0;
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
		for (uint32_t index = 0; index < _size; ++index)
		{
			_elements[index].~__TYPE__();
		}

		if (_elements != nullptr)
		{
			_allocator->Free(_elements);
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param values
	/// @return
	template<typename __TYPE__>
	Vector<__TYPE__>& Vector<__TYPE__>::operator=(std::initializer_list<__TYPE__> values)
	{
		uint32_t valueCount = static_cast<uint32_t>(values.size());

		if (_capacity >= valueCount)
		{
			if (_size >= valueCount)
			{
				for (uint32_t index = 0; index < valueCount; ++index)
				{
					_elements[index] = values.begin()[index];
				}

				for (uint32_t index = valueCount; index < _size; ++index)
				{
					_elements[index].~__TYPE__();
				}
			}
			else
			{
				for (uint32_t index = 0; index < _size; ++index)
				{
					_elements[index] = values.begin()[index];
				}

				for (uint32_t index = _size; index < valueCount; ++index)
				{
					New(&_elements[index], values.begin()[index]);
				}
			}

			_size = valueCount;
		}
		else
		{
			if (_elements != nullptr)
			{
				for (uint32_t index = 0; index < _size; ++index)
				{
					_elements[index].~__TYPE__();
				}

				_allocator->Free(_elements);
			}

			_capacity = valueCount;
			_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

			for (uint32_t index = 0; index < valueCount; ++index)
			{
				New(&_elements[index], values.begin()[index]);
			}

			_size = valueCount;
		}

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	/// @return
	template<typename __TYPE__>
	Vector<__TYPE__>& Vector<__TYPE__>::operator=(const Vector& vector)
	{
		if (&vector != this)
		{
			if (_capacity >= vector._size)
			{
				if (_size >= vector._size)
				{
					for (uint32_t index = 0; index < vector._size; ++index)
					{
						_elements[index] = vector._elements[index];
					}

					for (uint32_t index = vector._size; index < _size; ++index)
					{
						_elements[index].~__TYPE__();
					}
				}
				else
				{
					for (uint32_t index = 0; index < _size; ++index)
					{
						_elements[index] = vector._elements[index];
					}

					for (uint32_t index = _size; index < vector._size; ++index)
					{
						New(&_elements[index], vector._elements[index]);
					}
				}

				_size = vector._size;
			}
			else
			{
				if (_elements != nullptr)
				{
					for (uint32_t index = 0; index < _size; ++index)
					{
						_elements[index].~__TYPE__();
					}

					_allocator->Free(_elements);
				}

				_capacity = vector._size;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				for (uint32_t index = 0; index < vector._size; ++index)
				{
					New(&_elements[index], vector._elements[index]);
				}

				_size = vector._size;
			}
		}

		return *this;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param vector
	/// @return
	template<typename __TYPE__>
	Vector<__TYPE__>& Vector<__TYPE__>::operator=(Vector&& vector)
	{
		if (_elements != nullptr)
		{
			for (uint32_t index = 0; index < _size; ++index)
			{
				_elements[index].~__TYPE__();
			}

			_allocator->Free(_elements);
		}

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
	/// @param vector
	/// @return
	template<typename __TYPE__>
	bool Vector<__TYPE__>::operator!=(const Vector& vector) const
	{
		if (_size != vector._size)
		{
			return true;
		}

		for (uint32_t index = 0; index < _size; ++index)
		{
			if (_elements[index] != vector._elements[index])
			{
				return true;
			}
		}

		return false;
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param position
	/// @return
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::operator[](uint32_t position) const&
	{
		assert(position < _size);

		return _elements[position];
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param uiPosition
	/// @return
	template<typename __TYPE__>
	__TYPE__& Vector<__TYPE__>::operator[](uint32_t position) &
	{
		assert(position < _size);

		return _elements[position];
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param uiPosition
	/// @return
	template<typename __TYPE__>
	const __TYPE__& Vector<__TYPE__>::At(uint32_t position) const&
	{
		assert(position < _size);

		return _elements[position];
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param uiPosition
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
	/// @param uiCapacity
	template<typename __TYPE__>
	void Vector<__TYPE__>::Reserve(uint32_t uiCapacity)
	{
		if (uiCapacity > _capacity)
		{
			if (_elements == nullptr)
			{
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(uiCapacity * sizeof(__TYPE__), alignof(__TYPE__)));
				_capacity = uiCapacity;
			}
			else
			{
				if (!_allocator->Reallocate(_elements, uiCapacity * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(uiCapacity * sizeof(__TYPE__), alignof(__TYPE__)));

					for (uint32_t index = 0; index < _size; ++index)
					{
						New(&elements[index], std::move(_elements[index]));

						_elements[index].~__TYPE__();
					}

					_allocator->Free(_elements);

					_elements = elements;
				}
				_capacity = uiCapacity;
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
			_capacity = _size;
		}
		else
		{
			if (_capacity > _size)
			{
				if (!_allocator->Reallocate(_elements, _size * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_size * sizeof(__TYPE__), alignof(__TYPE__)));

					for (uint32_t index = 0; index < _size; ++index)
					{
						New(&elements[index], std::move(_elements[index]));

						_elements[index].~__TYPE__();
					}

					_allocator->Free(_elements, alignof(__TYPE__));

					_elements = elements;
				}
				_capacity = _size;
			}
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param size
	template<typename __TYPE__>
	void Vector<__TYPE__>::Resize(uint32_t size)
	{
		if (size != _size)
		{
			if (size > _size)
			{
				if (size <= _capacity)
				{
					for (uint32_t index = _size; index < size; ++index)
					{
						New(&_elements[index]);
					}

					_size = size;
				}
				else
				{
					if (_elements == nullptr)
					{
						_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(size * sizeof(__TYPE__), alignof(__TYPE__)));

						for (uint32_t index = 0; index < size; ++index)
						{
							New(&_elements[index]);
						}

						_size = size;
						_capacity = size;
					}
					else
					{
						if (!_allocator->Reallocate(_elements, size * sizeof(__TYPE__), alignof(__TYPE__)))
						{
							__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(size * sizeof(__TYPE__), alignof(__TYPE__)));

							for (uint32_t index = 0; index < _size; ++index)
							{
								New(&elements[index], static_cast<__TYPE__&&>(_elements[index]));

								_elements[index].~__TYPE__();
							}

							_allocator->Free(_elements);

							_elements = elements;
						}

						for (uint32_t index = _size; index < size; ++index)
						{
							New(&_elements[index]);
						}
						_size = size;
						_capacity = size;
					}
				}
			}
			else
			{
				if (_size > 0)
				{
					for (uint32_t index = _size - 1; index > size; --index) // > to avoid infinite loop if _size is 1 and size is 0
					{
						_elements[index].~__TYPE__();
					}

					_elements[size].~__TYPE__();
				}

				_size = size;
			}
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param size
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::Resize(uint32_t size, const __TYPE__& value)
	{
		if (size != _size)
		{
			if (size > _size)
			{
				if (size <= _capacity)
				{
					for (uint32_t index = _size; index < size; ++index)
					{
						New(&_elements[index], value);
					}

					_size = size;
				}
				else
				{
					if (_elements == nullptr)
					{
						_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(size * sizeof(__TYPE__), alignof(__TYPE__)));

						for (uint32_t index = 0; index < size; ++index)
						{
							New(&_elements[index], value);
						}

						_size = size;
						_capacity = size;
					}
					else
					{
						if (!_allocator->Reallocate(_elements, size * sizeof(__TYPE__), alignof(__TYPE__)))
						{
							__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(size * sizeof(__TYPE__), alignof(__TYPE__)));

							for (uint32_t index = 0; index < _size; ++index)
							{
								New(&elements[index], static_cast<__TYPE__&&>(_elements[index]));

								_elements[index].~__TYPE__();
							}

							_allocator->Free(_elements);

							_elements = elements;
						}

						for (uint32_t index = _size; index < size; ++index)
						{
							New(&_elements[index], value);
						}

						_size = size;
						_capacity = size;
					}
				}
			}
			else
			{
				if (_size > 0)
				{
					for (uint32_t index = _size - 1; index > size; --index) // > to avoid infinite loop if _size is 1 and size is 0
					{
						_elements[index].~__TYPE__();
					}

					_elements[size].~__TYPE__();
				}

				_size = size;
			}
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::PushBack(const __TYPE__& value)
	{
		if (_size < _capacity)
		{
			New(&_elements[_size], value);
			++_size;
		}
		else
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				_size = 1;
				New(&_elements[0], value);
			}
			else
			{
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));

					for (uint32_t index = 0; index < _size; ++index)
					{
						New(&elements[index], std::move(_elements[index]));
						_elements[index].~__TYPE__();
					}

					_allocator->Free(_elements);

					_elements = elements;
				}

				New(&_elements[_size], value);

				_capacity = _capacity * 2;
				++_size;
			}
		}
	}

	/// @brief
	/// @tparam __TYPE__
	/// @param value
	template<typename __TYPE__>
	void Vector<__TYPE__>::PushBack(__TYPE__&& value)
	{
		if (_size < _capacity)
		{
			New(&_elements[_size], std::move(value));
			++_size;
		}
		else
		{
			if (_elements == nullptr)
			{
				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				New(&_elements[_size], std::move(value));
				++_size;
			}
			else
			{
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));

					for (uint32_t index = 0; index < _size; ++index)
					{
						New(&elements[index], std::move(_elements[index]));

						_elements[index].~__TYPE__();
					}

					_allocator->Free(_elements);

					_elements = elements;
				}

				New(&_elements[_size], std::move(value));

				_capacity = _capacity * 2;
				++_size;
			}
		}
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

		_elements[_size - 1]
			.~__TYPE__(); // std::is_destructible should be called but is it returns false it means that the destructor is deleted and thus it generated a compilation error...

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
		if (_size < _capacity)
		{
			if (iterator.element < _elements + _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = &_elements[_size - 1]; element > iterator.element; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + (iterator.element - _elements))->~__TYPE__();
			}

			New(_elements + (iterator.element - _elements), value);
			++_size;

			return Iterator(const_cast<typename Iterator::pointer>(iterator.element));
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(iterator.element == nullptr);

				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				New(&_elements[_size], std::move(value));
				++_size;

				return Iterator(_elements);
			}
			else
			{
				assert(iterator.element <= _elements + _size);

				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* newElements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));
					__TYPE__* newElement = newElements;

					for (__TYPE__* element = _elements; element < iterator.element; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					__TYPE__* pFirstElement = newElement;
					New(newElement, value);
					++newElement;

					for (__TYPE__* element = _elements + (iterator.element - _elements); element < _elements + _size; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					++_size;

					_allocator->Free(_elements);

					_elements = newElements;
					_capacity = _capacity * 2;

					return Iterator(pFirstElement);
				}
				else
				{
					for (__TYPE__* element = _elements + _size; element > iterator.element; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = const_cast<__TYPE__*>(iterator.element);
					New(newElement, value);

					++_size;
					_capacity = _capacity * 2;

					return Iterator(newElement);
				}
			}
		}
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Insert(uint32_t index, const __TYPE__& value)
	{
		if (_size < _capacity)
		{
			if (index < _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = &_elements[_size - 1]; element > _elements + index; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + index)->~__TYPE__();
			}

			New(_elements + index, value);
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

				New(&_elements[_size], std::move(value));
				++_size;

				return 0;
			}
			else
			{
				assert(index <= _size);

				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* newElements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));
					__TYPE__* newElement = newElements;

					for (__TYPE__* element = _elements; element < _elements + index; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					__TYPE__* pFirstElement = newElement;
					New(newElement, value);
					++newElement;

					for (__TYPE__* element = _elements + index; element < _elements + _size; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					++_size;

					_allocator->Free(_elements);

					_elements = newElements;
					_capacity = _capacity * 2;

					return static_cast<uint32_t>(pFirstElement - _elements);
				}
				else
				{
					for (__TYPE__* element = _elements + _size; element > _elements + index; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = _elements + index;
					New(newElement, value);

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
		if (_size < _capacity)
		{
			assert(iterator.element <= _elements + _size);

			if (iterator.element < _elements + _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = _elements + _size - 1; element > iterator.element; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + (iterator.element - _elements))->~__TYPE__();
			}

			New(_elements + (iterator.element - _elements), std::move(value));
			++_size;

			return Iterator(const_cast<typename Iterator::pointer>(iterator.element));
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(iterator.element == nullptr);

				_capacity = 1;
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));

				New(&_elements[_size], std::move(value));
				++_size;

				return Iterator(_elements);
			}
			else
			{
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					assert(iterator.element <= _elements + _size);

					__TYPE__* newElements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));
					__TYPE__* newElement = newElements;
					for (__TYPE__* element = _elements; element < iterator.element; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					__TYPE__* pFirstElement = newElement;
					New(newElement, std::move(value));
					++newElement;

					for (__TYPE__* element = _elements + (iterator.element - _elements); element < _elements + _size; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					++_size;

					_allocator->Free(_elements);

					_elements = newElements;
					_capacity = _capacity * 2;

					return Iterator(pFirstElement);
				}
				else
				{
					for (__TYPE__* element = _elements + _size; element > iterator.element; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = const_cast<__TYPE__*>(iterator.element);
					New(newElement, std::move(value));

					++_size;
					_capacity = _capacity * 2;

					return Iterator(newElement);
				}
			}
		}
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Insert(uint32_t index, __TYPE__&& value)
	{
		if (_size < _capacity)
		{
			assert(index <= _size);

			if (index < _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = _elements + _size - 1; element > _elements + index; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + (_elements + index - _elements))->~__TYPE__();
			}

			New(_elements + (_elements + index - _elements), std::move(value));
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

				New(&_elements[_size], std::move(value));
				++_size;

				return 0;
			}
			else
			{
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					assert(_elements + index <= _elements + _size);

					__TYPE__* newElements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));
					__TYPE__* newElement = newElements;
					for (__TYPE__* element = _elements; element < _elements + index; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					__TYPE__* pFirstElement = newElement;
					New(newElement, std::move(value));
					++newElement;

					for (__TYPE__* element = _elements + index; element < _elements + _size; ++element)
					{
						New(newElement, std::move(*element));
						++newElement;
						element->~__TYPE__();
					}

					++_size;

					_allocator->Free(_elements);

					_elements = newElements;
					_capacity = _capacity * 2;

					return static_cast<uint32_t>(pFirstElement - _elements);
				}
				else
				{
					for (__TYPE__* element = _elements + _size; element > _elements + index; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = _elements + index;
					New(newElement, std::move(value));

					++_size;
					_capacity = _capacity * 2;

					return static_cast<uint32_t>(newElement - _elements);
				}
			}
		}
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

		if (first == last)
		{
			return Iterator(const_cast<typename Iterator::pointer>(iterator.element));
		}
		uint32_t elementToInsertCount = static_cast<uint32_t>(last.element - first.element);
		if (_size + elementToInsertCount <= _capacity)
		{
			for (__TYPE__* element = _elements + _size - 1; element >= iterator.element; --element)
			{
				New(element + elementToInsertCount, std::move(*element));
				element->~__TYPE__();
			}
			for (__TYPE__* element = const_cast<__TYPE__*>(iterator.element); element < iterator.element + elementToInsertCount; ++element)
			{
				New(element, *first.element);
				++first;
			}
			_size += elementToInsertCount;

			return Iterator(const_cast<typename Iterator::pointer>(iterator.element));
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(iterator.element == nullptr);

				_capacity = _size + elementToInsertCount; // Should be better to set capacity to a power of two ?
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
				__TYPE__* elements = _elements;
				for (__TYPE__* element = const_cast<__TYPE__*>(first.element); element < last.element; ++element)
				{
					New(elements, std::move(*element));
					++elements;
				}
				_size += elementToInsertCount;

				return Iterator(_elements);
			}
			else
			{
				assert(iterator.element <= _elements + _size);

				__TYPE__* newElements = reinterpret_cast<__TYPE__*>(
					_allocator->Allocate((_size + elementToInsertCount) * sizeof(__TYPE__), alignof(__TYPE__))); // Should be better to set capacity to a power of two ?
				__TYPE__* newElement = newElements;
				for (__TYPE__* element = _elements; element < iterator.element; ++element)
				{
					New(newElement, std::move(*element));
					++newElement;
					element->~__TYPE__();
				}
				__TYPE__* pFirstElement = newElement;
				for (__TYPE__* element = const_cast<__TYPE__*>(first.element); element < last.element; ++element)
				{
					New(newElement, std::move(*element));
					++newElement;
				}
				for (__TYPE__* element = _elements + (iterator.element - _elements); element < _elements + _size; ++element)
				{
					New(newElement, std::move(*element));
					++newElement;
					element->~__TYPE__();
				}

				_allocator->Free(_elements);

				_elements = newElements;
				_size += elementToInsertCount;
				_capacity = _size;

				return Iterator(pFirstElement);
			}
		}
	}

	template<typename __TYPE__>
	template<typename __ITERATOR__>
	uint32_t Vector<__TYPE__>::Insert(uint32_t index, __ITERATOR__ first, __ITERATOR__ last)
	{
		static_assert(!(std::is_same_v<__ITERATOR__, ReverseIterator> || std::is_same_v<__ITERATOR__, ConstReverseIterator>),
		              "Insert Range with reverse iterators is not supported");

		if (first == last)
		{
			return index;
		}
		uint32_t elementToInsertCount = static_cast<uint32_t>(last.element - first.element);
		if (_size + elementToInsertCount <= _capacity)
		{
			for (__TYPE__* element = _elements + _size - 1; element >= _elements + index; --element)
			{
				New(element + elementToInsertCount, std::move(*element));
				element->~__TYPE__();
			}
			for (__TYPE__* element = _elements + index; element < _elements + index + elementToInsertCount; ++element)
			{
				New(element, *first.element);
				++first;
			}
			_size += elementToInsertCount;

			return index;
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(index == 0);

				_capacity = _size + elementToInsertCount; // Should be better to set capacity to a power of two ?
				_elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * sizeof(__TYPE__), alignof(__TYPE__)));
				__TYPE__* elements = _elements;
				for (__TYPE__* element = const_cast<__TYPE__*>(first.element); element < last.element; ++element)
				{
					New(elements, std::move(*element));
					++elements;
				}
				_size += elementToInsertCount;

				return 0;
			}
			else
			{
				assert(_elements + index <= _elements + _size);

				__TYPE__* newElements = reinterpret_cast<__TYPE__*>(
					_allocator->Allocate((_size + elementToInsertCount) * sizeof(__TYPE__), alignof(__TYPE__))); // Should be better to set capacity to a power of two ?
				__TYPE__* newElement = newElements;
				for (__TYPE__* element = _elements; element < _elements + index; ++element)
				{
					New(newElement, std::move(*element));
					++newElement;
					element->~__TYPE__();
				}
				__TYPE__* pFirstElement = newElement;
				for (__TYPE__* element = const_cast<__TYPE__*>(first.element); element < last.element; ++element)
				{
					New(newElement, std::move(*element));
					++newElement;
				}
				for (__TYPE__* element = _elements + index; element < _elements + _size; ++element)
				{
					New(newElement, std::move(*element));
					++newElement;
					element->~__TYPE__();
				}

				_allocator->Free(_elements);

				_elements = newElements;
				_size += elementToInsertCount;
				_capacity = _size;

				return static_cast<uint32_t>(pFirstElement - _elements);
			}
		}
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
				if (!_allocator->Reallocate(_elements, _capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)))
				{
					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));

					for (uint32_t index = 0; index < _size; ++index)
					{
						New(&elements[index], std::move(_elements[index]));
						_elements[index].~__TYPE__();
					}

					_allocator->Free(_elements);

					_elements = elements;
				}

				New(&_elements[_size], std::forward<__ARGUMENTS__>(arguments)...);
				++_size;
				_capacity = _capacity * 2;
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
			assert(iterator.element <= _elements + _size);

			if (iterator.element < _elements + _size)
			{
				New(&_elements[_size], std::move(_elements[_size - 1]));

				for (__TYPE__* element = &_elements[_size - 1]; element > iterator.element; --element)
				{
					*element = std::move(*(element - 1));
				}

				(_elements + (iterator.element - _elements))->~__TYPE__();
			}

			New(_elements + (iterator.element - _elements), std::forward<__ARGUMENTS__>(arguments)...);
			++_size;

			return Iterator(const_cast<typename Iterator::pointer>(iterator.element));
		}
		else
		{
			if (_elements == nullptr)
			{
				assert(iterator.element == nullptr);

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
					assert(iterator.element <= _elements + _size);

					__TYPE__* elements = reinterpret_cast<__TYPE__*>(_allocator->Allocate(_capacity * 2 * sizeof(__TYPE__), alignof(__TYPE__)));

					for (__TYPE__* element = _elements; element < iterator.element; ++element)
					{
						New(elements, std::move(*element));
						++elements;
						element->~__TYPE__();
					}

					__TYPE__* newElement = elements;
					New(elements, std::forward<__ARGUMENTS__>(arguments)...);
					++elements;

					for (__TYPE__* element = _elements + (iterator.element - _elements); element < _elements + _size; ++element)
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
					for (__TYPE__* element = _elements + _size; element > iterator.element; --element)
					{
						New(element, std::move(*(element - 1)));
						(element - 1)->~__TYPE__();
					}

					__TYPE__* newElement = const_cast<__TYPE__*>(iterator.element);
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
		assert(iterator.element < _elements + _size);

		for (__TYPE__* element = _elements + (iterator.element - _elements); element < _elements + _size - 1; ++element)
		{
			*element = std::move(*(element + 1));
		}

		_elements[_size - 1].~__TYPE__();
		--_size;

		return Iterator(const_cast<typename Iterator::pointer>(iterator.element));
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
		assert(first.element <= _elements + _size);
		assert(first.element <= last.element);

		if (first.element == last.element)
		{
			return Iterator(const_cast<typename Iterator::pointer>(last.element));
		}

		uint32_t  uiNewSize = _size - static_cast<uint32_t>(last.element - first.element);
		__TYPE__* pFirst = _elements + (first.element - _elements);

		for (__TYPE__* element = _elements + (last.element - _elements); element < _elements + _size; ++element)
		{
			*(pFirst) = std::move(*element);
			++pFirst;
		}

		for (__TYPE__* element = pFirst; element < _elements + _size; ++element)
		{
			element->~__TYPE__();
		}

		_size = uiNewSize;

		return Iterator(const_cast<typename Iterator::pointer>(first.element));
	}

	template<typename __TYPE__>
	uint32_t Vector<__TYPE__>::Erase(uint32_t index, uint32_t count)
	{
		assert(index <= _size);

		if (!count)
		{
			return index;
		}

		uint32_t  uiNewSize = _size - count;
		__TYPE__* pFirst = _elements + index;

		for (__TYPE__* element = _elements + index + count; element < _elements + _size; ++element)
		{
			*(pFirst) = std::move(*element);
			++pFirst;
		}

		for (__TYPE__* element = pFirst; element < _elements + _size; ++element)
		{
			element->~__TYPE__();
		}

		_size = uiNewSize;

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
		uint32_t   uiCapacity = _capacity;
		uint32_t   size = _size;
		Allocator* allocator = _allocator;

		_elements = vector._elements;
		_capacity = vector._capacity;
		_size = vector._size;
		_allocator = vector._allocator;

		vector._elements = elements;
		vector._capacity = uiCapacity;
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
		assert((itFirst.element < _elements + _size) && (itSecond.element < _elements + _size));

		__TYPE__ value(std::move(const_cast<__TYPE__&>(*itFirst.element)));
		*(_elements + (itFirst.element - _elements)) = std::move(*const_cast<__TYPE__*>(itSecond.element));
		*(_elements + (itSecond.element - _elements)) = std::move(value);
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
		assert(iterator.element < _elements + _size);

		if (iterator.element < _elements + _size - 1)
		{
			Swap(iterator, ConstIterator(_elements + _size - 1));
		}

		if (iterator.element < _elements + _size)
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
}
