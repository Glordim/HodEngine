
namespace hod
{
	/// @brief 
	/// @tparam __TRAIT_TYPE__ 
	/// @tparam ...Args 
	/// @param ...args 
	template<typename __TRAIT_TYPE__, typename... Args>
	__TRAIT_TYPE__* ReflectionProperty::AddTrait(Args&&... args)
	{
		__TRAIT_TYPE__* trait = new __TRAIT_TYPE__(std::forward<Args>(args)...);
		AddTrait(trait);
		return trait;
	}

	/// @brief 
	/// @tparam _Trait_ 
	/// @return 
	template<typename _Trait_>
	_Trait_* ReflectionProperty::FindTrait() const
	{
		return static_cast<_Trait_*>(FindTrait(_Trait_::GetMetaTypeStatic()));
	}
}
