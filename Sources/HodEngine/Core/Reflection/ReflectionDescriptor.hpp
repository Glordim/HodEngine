#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/FunctionPtr.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <cstdint>
#include <string_view>

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/TypeTrait.hpp"

namespace hod::inline core
{
	class ReflectionTrait;
	class ReflectionProperty;

	///@brief
	class HOD_CORE_API ReflectionDescriptor
	{
	public:
		struct Data
		{
			std::string_view                            _name;
			ReflectionDescriptor*                       _parent = nullptr;
			FunctionPtr<void*()>                        _allocateFunction = nullptr;
			FunctionPtr<void(void*)>                    _deleteFunction = nullptr;
			FunctionPtr<bool(const void*, const void*)> _compareFunction = nullptr;
		};

		template<typename _Class_, typename _ParentClass_ = void>
		static Data GenerateReflectionData(const std::string_view& name);

	public:
		ReflectionDescriptor() = default;
		ReflectionDescriptor(const Data& data);
		ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent);
		ReflectionDescriptor(const ReflectionDescriptor& copy) = default;
		ReflectionDescriptor(ReflectionDescriptor&& move) = default;
		~ReflectionDescriptor();

		ReflectionDescriptor& operator=(const ReflectionDescriptor& copy) = default;
		ReflectionDescriptor& operator=(ReflectionDescriptor&& move) = default;

	public:
		void Init(const Data& data);

		const Vector<ReflectionTrait*>&    GetTraits() const;
		const Vector<ReflectionProperty*>& GetProperties() const;

		template<typename _Trait_, typename... Args>
		_Trait_* AddTrait(Args&&... args);
		void     AddTrait(ReflectionTrait* trait);

		template<typename _Property_, typename... Args>
		_Property_* AddProperty(Args&&... args);
		void        AddProperty(ReflectionProperty* property);

		template<typename _Trait_>
		_Trait_*         FindTrait(bool fallbackOnParent = true) const;
		ReflectionTrait* FindTrait(RttiType rttiType, bool fallbackOnParent = true) const;

		template<typename _Property_>
		_Property_*         FindProperty(const std::string_view& name, bool fallbackOnParent = true) const;
		ReflectionProperty* FindProperty(const std::string_view& name, bool fallbackOnParent = true) const;

		template<typename _ObjectType_>
		void CollectObjectProperties(Vector<_ObjectType_*>& collectedInstances, void* instance);
		void CollectObjectProperties(const ReflectionDescriptor& reflectionDescriptor, Vector<void*>& collectedInstances, void* instance);

		template<typename _Trait_>
		void RemoveTrait();
		void RemoveTrait(RttiType rttiType);

		void* CreateInstance() const;
		void  DeleteInstance(void* instance);

		template<typename _Type_>
		_Type_* CreateInstance() const;

		void Copy(const void* sourceInstance, void* destinationInstance);

		bool Compare(const void* left, const void* right) const;

		uint64_t      GetType() const;
		const String& GetTypeName() const;

		ReflectionDescriptor* GetParent() const;
		bool                  IsCompatible(const ReflectionDescriptor& descriptor) const;

		const String& GetDisplayName() const;

	private:
		String         _typeName;
		uint64_t       _type;
		mutable String _displayName;

		ReflectionDescriptor*                       _parent = nullptr;
		FunctionPtr<void*()>                        _allocateFunction = nullptr;
		FunctionPtr<void(void*)>                    _deleteFunction = nullptr;
		FunctionPtr<bool(const void*, const void*)> _compareFunction = nullptr;

		Vector<ReflectionTrait*>    _traits;
		Vector<ReflectionProperty*> _properties;
	};

	template<typename _Class_, typename _ParentClass_>
	ReflectionDescriptor::Data ReflectionDescriptor::GenerateReflectionData(const std::string_view& name)
	{
		Data data;
		data._name = name;

		if constexpr (std::is_same<_ParentClass_, void>::value == true)
		{
			data._parent = nullptr;
		}
		else
		{
			data._parent = &_ParentClass_::GetReflectionDescriptor();
		}

		if constexpr (std::is_abstract<_Class_>::value == true || std::is_default_constructible<_Class_>::value == false)
		{
			data._allocateFunction = nullptr;
			data._deleteFunction = nullptr;
		}
		else
		{
			data._allocateFunction = +[]() -> void* { return DefaultAllocator::GetInstance().New<_Class_>(); };
			data._deleteFunction = +[](void* instance) { hod::DefaultAllocator::GetInstance().Delete(static_cast<_Class_*>(instance)); };
		}

		if constexpr (HasEqualOperator<_Class_>::value)
		{
			data._compareFunction = [](const void* left, const void* right) { return *static_cast<const _Class_*>(left) == *static_cast<const _Class_*>(right); };
		}

		return data;
	}

	template<typename _Trait_, typename... Args>
	_Trait_* ReflectionDescriptor::AddTrait(Args&&... args)
	{
		_Trait_* trait = DefaultAllocator::GetInstance().New<_Trait_>(std::forward<Args>(args)...);
		AddTrait(trait);
		return trait;
	}

	template<typename _Trait_>
	void ReflectionDescriptor::RemoveTrait()
	{
		RemoveTrait(_Trait_::GetRttiTypeStatic());
	}

	/// @brief
	/// @tparam _Trait_
	/// @return
	template<typename _Trait_>
	_Trait_* ReflectionDescriptor::FindTrait(bool fallbackOnParent) const
	{
		return static_cast<_Trait_*>(FindTrait(_Trait_::GetRttiTypeStatic(), fallbackOnParent));
	}

	template<typename _Property_, typename... Args>
	_Property_* ReflectionDescriptor::AddProperty(Args&&... args)
	{
		_Property_* property = DefaultAllocator::GetInstance().New<_Property_>(std::forward<Args>(args)...);
		AddProperty(property);
		return property;
	}

	template<typename _Property_>
	_Property_* ReflectionDescriptor::FindProperty(const std::string_view& name, bool fallbackOnParent) const
	{
		return static_cast<_Property_*>(FindProperty(name, fallbackOnParent));
	}

	template<typename _Type_>
	_Type_* ReflectionDescriptor::CreateInstance() const
	{
		return static_cast<_Type_*>(CreateInstance());
	}

	template<typename _ObjectType_>
	void ReflectionDescriptor::CollectObjectProperties(Vector<_ObjectType_*>& collectedInstances, void* instance)
	{
		CollectObjectProperties(_ObjectType_::GetReflectionDescriptor(), (Vector<void*>&)collectedInstances, instance);
	}
}
