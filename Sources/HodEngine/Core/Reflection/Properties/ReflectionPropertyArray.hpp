#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/FunctionPtr.hpp"
#include <cassert>

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/TypeTrait.hpp"

namespace hod::inline core
{
	class ReflectionDescriptor;

	struct HOD_CORE_API Adapter
	{
	public:
		template<typename _type_>
		void AsRaw()
		{
			_getElementCountFunction = [](const void* /*instance*/) -> uint32_t { return sizeof(_type_) / sizeof(ElementType<_type_>::type); };

			_getElementAddressFunction = [](const void* instance, uint32_t index) -> void*
			{
				const typename ElementType<_type_>::type* address = static_cast<const typename ElementType<_type_>::type*>(instance);
				return address + index;
			};

			_insertElementFunction = nullptr;
			_removeElementFunction = nullptr;
			_clearFunction = nullptr;
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
			_clearFunction = nullptr;
		}

		template<typename _vector_>
		void AsVector()
		{
			_getElementCountFunction = [](const void* instance) -> uint32_t
			{
				const _vector_* array = static_cast<const _vector_*>(instance);
				return (uint32_t)array->Size();
			};

			_getElementAddressFunction = [](const void* instance, uint32_t index) -> void*
			{
				const _vector_* array = static_cast<const _vector_*>(instance);
				return (void*)(array->Data() + index);
			};

			_insertElementFunction = [](void* instance, uint32_t index)
			{
				_vector_*                     array = static_cast<_vector_*>(instance);
				typename _vector_::value_type item = typename _vector_::value_type();
				array->Insert(index, item);
			};

			_removeElementFunction = [](void* instance, uint32_t index)
			{
				_vector_* array = static_cast<_vector_*>(instance);
				array->Erase(index);
			};

			_clearFunction = [](void* instance)
			{
				_vector_* array = static_cast<_vector_*>(instance);
				array->Clear();
			};
		}

	public:
		FunctionPtr<uint32_t(const void*)>        _getElementCountFunction;
		FunctionPtr<void*(const void*, uint32_t)> _getElementAddressFunction;
		FunctionPtr<void(void*, uint32_t)>        _insertElementFunction;
		FunctionPtr<void(void*, uint32_t)>        _removeElementFunction;
		FunctionPtr<void(void*)>                  _clearFunction;
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
		RTTI(ReflectionPropertyArray, ReflectionProperty)

	public:
		ReflectionPropertyArray(Adapter adapter, ReflectionPropertyVariable::Type type, uint32_t offset, const char* name,
		                        ReflectionDescriptor* elementReflectionDescriptor = nullptr);
		ReflectionPropertyArray(const ReflectionPropertyArray& copy) = default;
		ReflectionPropertyArray(ReflectionPropertyArray&& move) = default;
		~ReflectionPropertyArray() = default;

		ReflectionPropertyArray& operator=(const ReflectionPropertyArray& copy) = default;
		ReflectionPropertyArray& operator=(ReflectionPropertyArray&& move) = default;

	public:
		ReflectionPropertyVariable::Type GetType() const;

		template<typename _type_>
		_type_ GetValue(const void* instance, uint32_t index) const;

		template<typename _type_>
		void SetValue(void* instance, uint32_t index, _type_ value);

		ReflectionDescriptor* GetElementReflectionDescriptor() const;

		uint32_t GetElementCount(const void* instance) const;
		void     InsertElement(void* instance, uint32_t index) const;
		void     RemoveElement(void* instance, uint32_t index) const;
		void     Clear(void* instance) const;

		void Copy(const void* sourceInstance, void* destinationInstance) const override;

	private:
		ReflectionPropertyVariable::Type _type;

		ReflectionDescriptor* _elementReflectionDescriptor = nullptr;

		Adapter _adapter;
	};

	/// @brief
	/// @tparam _type_
	/// @param instance
	/// @param index
	/// @return
	template<typename _type_>
	_type_ ReflectionPropertyArray::GetValue(const void* instance, uint32_t index) const
	{
		const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
		const void*    arrayAddress = reinterpret_cast<const void*>(instanceAddress + _offset);
		const void*    variableAddress = _adapter._getElementAddressFunction((void*)arrayAddress, index);

		if constexpr (std::is_same<std::remove_cv_t<_type_>, bool>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Bool);
			return *reinterpret_cast<const bool*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, int8_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Int8);
			return *reinterpret_cast<const int8_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, int16_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Int16);
			return *reinterpret_cast<const int16_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, int32_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Int32);
			return *reinterpret_cast<const int32_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, int64_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Int64);
			return *reinterpret_cast<const int64_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, uint8_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::UInt8);
			return *reinterpret_cast<const uint8_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, uint16_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::UInt16);
			return *reinterpret_cast<const uint16_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, uint32_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::UInt32);
			return *reinterpret_cast<const uint32_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, uint64_t>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::UInt64);
			return *reinterpret_cast<const uint64_t*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, float>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Float32);
			return *reinterpret_cast<const float*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, double>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Float64);
			return *reinterpret_cast<const double*>(variableAddress);
		}
		else if constexpr (std::is_same<std::remove_cv_t<_type_>, void*>::value)
		{
			assert(_type == ReflectionPropertyVariable::Type::Object);
			return (_type_)(variableAddress);
		}
		else
		{
			assert(false);
			return _type_();
		}
	}

	/// @brief
	/// @tparam _type_
	/// @param instance
	/// @param index
	/// @param value
	template<typename _type_>
	void ReflectionPropertyArray::SetValue(void* instance, uint32_t index, _type_ value)
	{
		uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
		void*    arrayAddress = reinterpret_cast<void*>(instanceAddress + _offset);
		void*    variableAddress = _adapter._getElementAddressFunction(arrayAddress, index);

		switch (_type)
		{
			case ReflectionPropertyVariable::Type::Bool: *reinterpret_cast<bool*>(variableAddress) = static_cast<bool>(value); break;
			case ReflectionPropertyVariable::Type::Int8: *reinterpret_cast<int8_t*>(variableAddress) = static_cast<int8_t>(value); break;
			case ReflectionPropertyVariable::Type::Int16: *reinterpret_cast<int16_t*>(variableAddress) = static_cast<int16_t>(value); break;
			case ReflectionPropertyVariable::Type::Int32: *reinterpret_cast<int32_t*>(variableAddress) = static_cast<int32_t>(value); break;
			case ReflectionPropertyVariable::Type::Int64: *reinterpret_cast<int64_t*>(variableAddress) = static_cast<int64_t>(value); break;
			case ReflectionPropertyVariable::Type::UInt8: *reinterpret_cast<uint8_t*>(variableAddress) = static_cast<uint8_t>(value); break;
			case ReflectionPropertyVariable::Type::UInt16: *reinterpret_cast<uint16_t*>(variableAddress) = static_cast<uint16_t>(value); break;
			case ReflectionPropertyVariable::Type::UInt32: *reinterpret_cast<uint32_t*>(variableAddress) = static_cast<uint32_t>(value); break;
			case ReflectionPropertyVariable::Type::UInt64: *reinterpret_cast<uint64_t*>(variableAddress) = static_cast<uint64_t>(value); break;
			case ReflectionPropertyVariable::Type::Float32: *reinterpret_cast<float*>(variableAddress) = static_cast<float>(value); break;
			case ReflectionPropertyVariable::Type::Float64:
				*reinterpret_cast<double*>(variableAddress) = static_cast<double>(value);
				break;
				// case ReflectionPropertyVariable::Type::String: *reinterpret_cast<String*>(variableAddress) = value; break;

			default: assert(false); break;
		}
	}
}
