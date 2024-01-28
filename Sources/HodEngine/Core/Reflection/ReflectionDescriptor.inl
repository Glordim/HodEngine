
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
			data._parent = _ParentClass_::GetReflectionDescriptor();
		}

		if constexpr (std::is_abstract<_Class_>::value == true || std::is_default_constructible<_Class_>::value == false)
		{		
			data._allocateFunction = nullptr;
			data._sharedAllocateFunction = nullptr;
		}
		else
		{
			data._allocateFunction = [](){ return new _Class_(); };
			data._sharedAllocateFunction = [](){ return std::make_shared<_Class_>(); };
		}

		return data;
	}

	template<typename __TRAIT_TYPE__, typename... Args>
	__TRAIT_TYPE__* ReflectionDescriptor::AddTrait(Args&&... args)
	{
		__TRAIT_TYPE__* trait = new __TRAIT_TYPE__(std::forward<Args>(args)...);
		AddTrait(trait);
		return trait;
	}

	/// @brief 
	/// @tparam _Trait_ 
	/// @return 
	template<typename _Trait_>
	_Trait_* ReflectionDescriptor::FindTrait() const
	{
		return static_cast<_Trait_*>(FindTrait(_Trait_::GetMetaTypeStatic()));
	}

	template<typename __PROPERTY_TYPE__, typename... Args>
	__PROPERTY_TYPE__* ReflectionDescriptor::AddProperty(Args&&... args)
	{
		__PROPERTY_TYPE__* property = new __PROPERTY_TYPE__(std::forward<Args>(args)...);
		AddProperty(property);
		return property;
	}

	template<typename _Type_>
	_Type_* ReflectionDescriptor::CreateInstance() const
	{
		return static_cast<_Type_*>(CreateInstance());
	}

	template<typename _Type_>
	std::shared_ptr<_Type_> ReflectionDescriptor::CreateSharedInstance() const
	{
		return std::static_pointer_cast<_Type_>(CreateSharedInstance());
	}
}
