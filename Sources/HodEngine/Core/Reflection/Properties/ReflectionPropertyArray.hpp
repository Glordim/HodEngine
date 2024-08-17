#pragma once
#include "HodEngine/Core/Export.hpp"

#include <vector>
#include <functional>
#include <cassert>

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/TypeTrait.hpp"

namespace hod
{
	class ReflectionDescriptor;

	struct HOD_CORE_API Adapter
	{
	public:

		template<typename _type_>
		void AsRaw()
		{
			_getElementCountFunction = [](const void* instance) -> uint32_t
			{
				return sizeof(_type_) / sizeof(ElementType<_type_>::type);
			};

			_getElementAddressFunction = [](const void* instance, uint32_t index) -> void*
			{
				const typename ElementType<_type_>::type* address = static_cast<const typename ElementType<_type_>::type*>(instance);
				return address + index;
			};

			_insertElementFunction = nullptr;
			_removeElementFunction = nullptr;
		}

		template<typename _array_>
		void AsArray()
		{
			_getElementCountFunction = [](const void* instance) -> uint32_t
			{
				const _array_* array = static_cast<const _array_*>(instance);
				return array->size();
			};

			_getElementAddressFunction = [](const void* instance, uint32_t index) -> void*
			{
				const _array_* array = static_cast<const _array_*>(instance);
				return (void*)(array->data() + index);
			};

			_insertElementFunction = nullptr;
			_removeElementFunction = nullptr;
		}

		template<typename _vector_>
		void AsVector()
		{
			_getElementCountFunction = [](const void* instance) -> uint32_t
			{
				const _vector_* array = static_cast<const _vector_*>(instance);
				return (uint32_t)array->size();
			};

			_getElementAddressFunction = [](const void* instance, uint32_t index) -> void*
			{
				const _vector_* array = static_cast<const _vector_*>(instance);
				return (void*)(array->data() + index);
			};

			_insertElementFunction = [](void* instance, uint32_t index)
			{
				_vector_* array = static_cast<_vector_*>(instance);
				typename _vector_::value_type item = typename _vector_::value_type();
				array->insert(array->begin() + index, item);
			};

			_removeElementFunction = [](void* instance, uint32_t index)
			{
				_vector_* array = static_cast<_vector_*>(instance);
				array->erase(array->begin() + index);
			};
		}

	public:

		std::function<uint32_t(const void*)>		_getElementCountFunction;
		std::function<void*(const void*, uint32_t)>	_getElementAddressFunction;
		std::function<void(void*, uint32_t)>		_insertElementFunction;
		std::function<void(void*, uint32_t)>		_removeElementFunction;
	};

	template<typename _container_>
	Adapter MakeAdapter()
	{
		Adapter adapter;
		if constexpr (std::is_array_v<_container_>)
		{
			adapter.AsRaw<_container_>();
		}
		else if constexpr (IsArray<_container_>::value)
		{
			adapter.AsArray<_container_>();
		}
		else if constexpr (IsVector<_container_>::value)
		{
			adapter.AsVector<_container_>();
		}
		else
		{
			static_assert(always_false<_container_>);
		}
		return adapter;
	}

	///@brief 
	class HOD_CORE_API ReflectionPropertyArray : public ReflectionProperty
	{
		META_TYPE(ReflectionPropertyArray, ReflectionProperty)

	public:

												ReflectionPropertyArray(Adapter adapter, ReflectionPropertyVariable::Type type, uint32_t offset, const char* name, ReflectionDescriptor* elementReflectionDescriptor = nullptr);
												ReflectionPropertyArray(const ReflectionPropertyArray& copy) = default;
												ReflectionPropertyArray(ReflectionPropertyArray&& move) = default;
												~ReflectionPropertyArray() = default;

		ReflectionPropertyArray&				operator = (const ReflectionPropertyArray& copy) = default;
		ReflectionPropertyArray&				operator = (ReflectionPropertyArray&& move) = default;

	public:

		ReflectionPropertyVariable::Type		GetType() const;

		template<typename _type_>
		_type_									GetValue(const void* instance, uint32_t index) const;

		template<typename _type_>
		void									SetValue(void* instance, uint32_t index, _type_ value);

		ReflectionDescriptor*					GetElementReflectionDescriptor() const;

		uint32_t								GetElementCount(const void* instance) const;
		void									InsertElement(void* instance, uint32_t index) const;
		void									RemoveElement(void* instance, uint32_t index) const;

		void									Copy(const void* sourceInstance, void* destinationInstance) const override;

	private:

		ReflectionPropertyVariable::Type		_type;

		ReflectionDescriptor*					_elementReflectionDescriptor = nullptr;

		Adapter									_adapter;
	};
}

#include "ReflectionPropertyArray.inl"
