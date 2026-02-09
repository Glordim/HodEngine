
namespace hod
{
	template<typename _Class_, typename _ParentClass_>
	ReflectionDescriptor::Data ReflectionDescriptor::GenerateReflectionData(const std::string_view& name)
	{
		Data data;
		data._name = name;
		data._metaType = _Class_::GetMetaTypeStatic();

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
		RemoveTrait(_Trait_::GetMetaTypeStatic());
	}

	/// @brief
	/// @tparam _Trait_
	/// @return
	template<typename _Trait_>
	_Trait_* ReflectionDescriptor::FindTrait(bool fallbackOnParent) const
	{
		return static_cast<_Trait_*>(FindTrait(_Trait_::GetMetaTypeStatic(), fallbackOnParent));
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
