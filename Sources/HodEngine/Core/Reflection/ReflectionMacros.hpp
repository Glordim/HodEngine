#pragma once

#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"

#define REFLECTED_CLASS_NO_PARENT(CLASS)                                                             \
public:                                                                                              \
	using ReflectedClassType = CLASS; /* For HOD_PROPERTY */                                         \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)            \
	{                                                                                                \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS>(#CLASS)); \
		FillReflectionDescriptorUser(reflectionDescriptor);                                          \
	}                                                                                                \
                                                                                                     \
	static void FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor);       \
                                                                                                     \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                      \
	{                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                \
	}                                                                                                \
                                                                                                     \
	virtual ReflectionDescriptor& GetReflectionDescriptorV() const                                   \
	{                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                \
	}                                                                                                \
                                                                                                     \
	friend void DescribeClass(hod::ReflectionDescriptor& reflectionDescriptor, const CLASS*)         \
	{                                                                                                \
		CLASS::FillReflectionDescriptor(reflectionDescriptor);                                       \
	}

#define REFLECTED_CLASS_NO_VIRTUAL(CLASS)                                                            \
public:                                                                                              \
	using ReflectedClassType = CLASS; /* For HOD_PROPERTY */                                         \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)            \
	{                                                                                                \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS>(#CLASS)); \
		FillReflectionDescriptorUser(reflectionDescriptor);                                          \
	}                                                                                                \
                                                                                                     \
	static void FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor);       \
                                                                                                     \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                      \
	{                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                \
	}                                                                                                \
                                                                                                     \
	ReflectionDescriptor& GetReflectionDescriptorV() const                                           \
	{                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                \
	}                                                                                                \
                                                                                                     \
	friend void DescribeClass(hod::ReflectionDescriptor& reflectionDescriptor, const CLASS*)         \
	{                                                                                                \
		CLASS::FillReflectionDescriptor(reflectionDescriptor);                                       \
	}

// REFLECTED_CLASS_NO_VIRTUAL for a class deriving from another REFLECTED_CLASS_NO_VIRTUAL one: the parent's
// properties are reflected as well, still without any vtable.
#define REFLECTED_CLASS_NO_VIRTUAL_WITH_PARENT(CLASS, PARENT)                                                \
public:                                                                                                      \
	using ReflectedClassType = CLASS; /* For HOD_PROPERTY */                                                 \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)                    \
	{                                                                                                        \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS, PARENT>(#CLASS)); \
		FillReflectionDescriptorUser(reflectionDescriptor);                                                  \
	}                                                                                                        \
                                                                                                             \
	static void FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor);               \
                                                                                                             \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                              \
	{                                                                                                        \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                        \
	}                                                                                                        \
                                                                                                             \
	hod::ReflectionDescriptor& GetReflectionDescriptorV() const                                              \
	{                                                                                                        \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                        \
	}                                                                                                        \
                                                                                                             \
	friend void DescribeClass(hod::ReflectionDescriptor& reflectionDescriptor, const CLASS*)                 \
	{                                                                                                        \
		CLASS::FillReflectionDescriptor(reflectionDescriptor);                                               \
	}

#define REFLECTED_CLASS(CLASS, PARENT)                                                                       \
public:                                                                                                      \
	using ReflectedClassType = CLASS; /* For HOD_PROPERTY */                                                 \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)                    \
	{                                                                                                        \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS, PARENT>(#CLASS)); \
		FillReflectionDescriptorUser(reflectionDescriptor);                                                  \
	}                                                                                                        \
                                                                                                             \
	static void FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor);               \
                                                                                                             \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                              \
	{                                                                                                        \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                        \
	}                                                                                                        \
                                                                                                             \
	hod::ReflectionDescriptor& GetReflectionDescriptorV() const override                                     \
	{                                                                                                        \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                        \
	}                                                                                                        \
                                                                                                             \
	friend void DescribeClass(hod::ReflectionDescriptor& reflectionDescriptor, const CLASS*)                 \
	{                                                                                                        \
		CLASS::FillReflectionDescriptor(reflectionDescriptor);                                               \
	}

///@brief Its traits (HOD_TRAIT), its properties (HOD_PROPERTY)... A type without any: DESCRIBE_REFLECTED_CLASS(Type) {}. The descriptor
/// being built is the parameter reflectionDescriptor, which the HOD_ macros use (not ReflectedClassType::GetReflectionDescriptor(): it
/// would describe the class again)
#define DESCRIBE_REFLECTED_CLASS(__TYPE__) void __TYPE__::FillReflectionDescriptorUser([[maybe_unused]] hod::ReflectionDescriptor& reflectionDescriptor)

/// @brief In a DESCRIBE_REFLECTED_CLASS: a trait of the class, made of the arguments (HOD_TRAIT(ReflectionTraitIcon, ICON_MDI_CAMERA)).
/// Returns it. A property's: HOD_PROPERTY(_member)->AddTrait<Trait>(...)
#define HOD_TRAIT(TRAIT, ...) reflectionDescriptor.AddTrait<TRAIT>(__VA_ARGS__)

/// @brief In a DESCRIBE_REFLECTED_CLASS: the member of the class as a property, then its setter if any (&Class::SetValue). Returns the
/// ReflectionProperty* (->AddTrait<...>(...)). Its name is the member's, as written (_jumpPower): its identity, the key of its value in
/// the files, of its overrides, of its animation tracks... (the inspector shows GenerateDisplayName's: Jump Power)
#define HOD_PROPERTY(MEMBER, ...) hod::AddPropertyT(reflectionDescriptor, &ReflectedClassType::MEMBER, #MEMBER __VA_OPT__(, ) __VA_ARGS__)

/// @brief HOD_PROPERTY with its name given (a string literal): the member renamed, its data kept
#define HOD_PROPERTY_NAMED(MEMBER, NAME, ...) hod::AddPropertyT(reflectionDescriptor, &ReflectedClassType::MEMBER, NAME __VA_OPT__(, ) __VA_ARGS__)

#define REFLECTED_ENUM(__API__, __TYPE__) __API__ friend void DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__)
#define REFLECTED_ENUM2(__API__, __TYPE__) __API__ void DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__)

///@brief
#define DESCRIBE_REFLECTED_ENUM(__TYPE__, reflectionDescriptor) void DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__)
#define DESCRIBE_REFLECTED_ENUM2(__API__, __TYPE__, reflectionDescriptor) \
	template class __API__ hod::ReflectedEnum<__TYPE__>;                  \
	void                   DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__)
