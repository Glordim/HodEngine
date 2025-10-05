
namespace hod
{
	/// @brief
	/// @tparam Type
	template<typename Type>
	LinkedList<Type>::LinkedList()
	: _data(nullptr)
	{
	}

	/// @brief
	/// @tparam Type
	/// @param data
	template<typename Type>
	LinkedList<Type>::LinkedList(Type* data)
	: _data(data)
	{
	}

	/// @brief
	/// @tparam Type
	/// @return
	template<typename Type>
	inline LinkedList<Type>* LinkedList<Type>::Next() const
	{
		return _next;
	}

	/// @brief
	/// @tparam Type
	/// @return
	template<typename Type>
	LinkedList<Type>* LinkedList<Type>::Back() const
	{
		LinkedList<Type>* current = (LinkedList<Type>*)this;
		while (current->_next != nullptr)
		{
			current = current->_next;
		}
		return current;
	}

	/// @brief
	/// @tparam Type
	/// @param next
	template<typename Type>
	inline void LinkedList<Type>::InsertNext(LinkedList<Type>* next)
	{
		next->_next = _next;
		_next = next;
	}

	/// @brief
	/// @tparam Type
	/// @param element
	template<typename Type>
	inline void LinkedList<Type>::PushBack(LinkedList<Type>* element)
	{
		Back()->InsertNext(element);
	}

	/// @brief
	/// @tparam Type
	/// @return
	template<typename Type>
	inline Type* LinkedList<Type>::Data() const
	{
		return _data;
	}
}
