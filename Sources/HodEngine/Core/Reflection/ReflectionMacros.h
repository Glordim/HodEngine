#pragma once

#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"

///@brief Implement Getter method to retreive ReflectionDescriptor
#define GET_REFLECTION_DESCRIPTOR_METHOD(__REFLECTION_DESCRIPTOR_TYPE__)		\
static __REFLECTION_DESCRIPTOR_TYPE__*	GetReflectionDescriptor()				\
{																				\
	static __REFLECTION_DESCRIPTOR_TYPE__ reflectionDescriptor;					\
	return &reflectionDescriptor;												\
}																				\

///@brief Declare sub class used for Reflection
#define REFLECTED_CLASS_NO_PARENT(__TYPE__)											\
																					\
	BASE_META_TYPE(__TYPE__)														\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public core::ReflectionDescriptor		\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METHOD(__TYPE__##ReflectionDescriptor)				\
																					\
	virtual core::ReflectionDescriptor* GetReflectionDescriptorV() const			\
	{																				\
		return __TYPE__::GetReflectionDescriptor();									\
	}																				\
																					\
private:																			\

///@brief Declare sub class used for Reflection
#define REFLECTED_CLASS(__TYPE__)													\
																					\
	META_TYPE(__TYPE__)																\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public core::ReflectionDescriptor		\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METHOD(__TYPE__##ReflectionDescriptor)				\
																					\
	core::ReflectionDescriptor* GetReflectionDescriptorV() const override			\
	{																				\
		return __TYPE__::GetReflectionDescriptor();									\
	}																				\
																					\
private:																			\

///@brief 
#define DESCRIBE_REFLECTED_CLASS_NO_PARENT(__TYPE__)														\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()									\
: core::ReflectionDescriptor(core::ReflectionDescriptor::GenerateReflectionData<__TYPE__, void>(#__TYPE__))	\

///@brief 
#define DESCRIBE_REFLECTED_CLASS(__TYPE__, __PARENT__)																\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()											\
: core::ReflectionDescriptor(core::ReflectionDescriptor::GenerateReflectionData<__TYPE__, __PARENT__>(#__TYPE__))	\
