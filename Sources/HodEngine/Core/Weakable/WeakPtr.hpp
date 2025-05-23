#pragma once
#include "HodEngine/Core/Export.hpp"
#include <type_traits>

#include "HodEngine/Core/Weakable/WeakableAliveController.hpp"

namespace hod
{
	class Weakable;

	template <typename _Type_>
	concept IsDerivedFromWeakable = std::is_base_of_v<Weakable, _Type_>;

	template<IsDerivedFromWeakable _Type_>
	class WeakPtr
	{
	public:

		WeakPtr() = default;
		WeakPtr(_Type_* ptr);
		WeakPtr(_Type_& ptr);

		WeakPtr(const WeakPtr& copy);
		WeakPtr(WeakPtr&&) = delete;

		WeakPtr&	operator=(_Type_* ptr);
		WeakPtr&	operator=(_Type_& ptr);

		WeakPtr&	operator=(const WeakPtr& copy);
		WeakPtr&	operator=(WeakPtr&&) = delete;

		bool 		IsValid() const;

		_Type_*		Get() const;
		_Type_*		operator->() const;
		_Type_&		operator*() const;

	private:

		void		Reset(_Type_* weakable);

	private:

		_Type_*										_ptr = nullptr;
		std::shared_ptr<WeakableAliveController>	_aliveController = nullptr;
	};

	template<IsDerivedFromWeakable _Type_>
	WeakPtr<_Type_>::WeakPtr(_Type_* ptr)
	{
		Reset(ptr);
	}

	template<IsDerivedFromWeakable _Type_>
	WeakPtr<_Type_>::WeakPtr(_Type_& ptr)
	{
		Reset(&ptr);
	}

	template<IsDerivedFromWeakable _Type_>
	WeakPtr<_Type_>::WeakPtr(const WeakPtr<_Type_>& copy)
	{
		Reset(copy.Get());
	}

	template<IsDerivedFromWeakable _Type_>
	WeakPtr<_Type_>& WeakPtr<_Type_>::operator=(_Type_* ptr)
	{
		Reset(ptr);
		return *this;
	}

	template<IsDerivedFromWeakable _Type_>
	WeakPtr<_Type_>& WeakPtr<_Type_>::operator=(_Type_& ptr)
	{
		Reset(&ptr);
		return *this;
	}

	template<IsDerivedFromWeakable _Type_>
	WeakPtr<_Type_>& WeakPtr<_Type_>::operator=(const WeakPtr<_Type_>& copy)
	{
		Reset(copy.Get());
		return *this;
	}

	template<IsDerivedFromWeakable _Type_>
	void WeakPtr<_Type_>::Reset(_Type_* weakable)
	{
		if (weakable)
		{
			_ptr = weakable;
			_aliveController = weakable->GetAliveController();
		}
		else
		{
			_ptr = nullptr;
			_aliveController = nullptr;
		}
	}

	template<IsDerivedFromWeakable _Type_>
	bool WeakPtr<_Type_>::IsValid() const
	{
		return (_aliveController && _aliveController->IsAlive());
	}

	template<IsDerivedFromWeakable _Type_>
	_Type_* WeakPtr<_Type_>::Get() const
	{
		return IsValid() ? _ptr : nullptr;
	}

	template<IsDerivedFromWeakable _Type_>
	_Type_* WeakPtr<_Type_>::operator->() const
	{
		return Get();
	}

	template<IsDerivedFromWeakable _Type_>
	_Type_& WeakPtr<_Type_>::operator*() const
	{
		return *Get();
	}
}
