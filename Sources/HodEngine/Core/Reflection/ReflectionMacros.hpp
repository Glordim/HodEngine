#pragma once

#include "HodEngine/Core/Reflection/EnumDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"

#define REFLECTED_CLASS_NO_PARENT(CLASS)                                                             \
	BASE_META_TYPE(CLASS)                                                                            \
public:                                                                                              \
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
	}

#define REFLECTED_CLASS_NO_VIRTUAL(CLASS)                                                            \
	META_TYPE_NO_VIRTUAL(CLASS)                                                                      \
public:                                                                                              \
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
	}

#define REFLECTED_CLASS(CLASS, PARENT)                                                                       \
	META_TYPE(CLASS, PARENT)                                                                                 \
public:                                                                                                      \
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
	}

///@brief
#define DESCRIBE_REFLECTED_CLASS(__TYPE__, reflectionDescriptor) void __TYPE__::FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor)

#define REFLECTED_ENUM(__API__, __TYPE__) __API__ friend void DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__);
#define REFLECTED_ENUM2(__API__, __TYPE__) __API__ void DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__);

///@brief
#define DESCRIBE_REFLECTED_ENUM(__TYPE__, reflectionDescriptor) void DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__)
#define DESCRIBE_REFLECTED_ENUM2(__API__, __TYPE__, reflectionDescriptor) \
	template class __API__ hod::ReflectedEnum<__TYPE__>;                  \
	void                   DescribeEnum(hod::EnumDescriptor& reflectionDescriptor, __TYPE__)
