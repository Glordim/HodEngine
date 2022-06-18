#pragma once

#include "ReflectionDescriptor.h"

///@brief Implement Getter method to retreive ReflectionDescriptor
#define GET_REFLECTION_DESCRIPTOR_METhod(__REFLECTION_DESCRIPTOR_TYPE__)		\
static __REFLECTION_DESCRIPTOR_TYPE__*	GetReflectionDescriptor()				\
{																				\
	static __REFLECTION_DESCRIPTOR_TYPE__ reflectionDescriptor;					\
	return &reflectionDescriptor;												\
}																				\

///@brief Declare sub class used for Reflection
#define REFLECTED_ABSTRACT_CLASS(__TYPE__)										\
																				\
public:																			\
																				\
	class __TYPE__##ReflectionDescriptor : public CORE::ReflectionDescriptor	\
	{																			\
		public:																	\
																				\
			__TYPE__##ReflectionDescriptor();									\
	};																			\
																				\
	GET_REFLECTION_DESCRIPTOR_METhod(__TYPE__##ReflectionDescriptor)			\
																				\
private:																		\

///@brief Declare sub class used for Reflection
#define REFLECTED_CLASS(__TYPE__)												\
																				\
public:																			\
																				\
	class __TYPE__##ReflectionDescriptor : public CORE::ReflectionDescriptor	\
	{																			\
		public:																	\
																				\
			__TYPE__##ReflectionDescriptor();									\
	};																			\
																				\
	GET_REFLECTION_DESCRIPTOR_METhod(__TYPE__##ReflectionDescriptor)			\
																				\
private:																		\

///@brief Declare sub class used for Reflection
#define REFLECTED_ABSTRACT_DERIVED_CLASS(__TYPE__, __PARENT__)						\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public CORE::ReflectionDescriptor		\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METhod(__TYPE__##ReflectionDescriptor)				\
																					\
private:																			\

///@brief Declare sub class used for Reflection
#define REFLECTED_DERIVED_CLASS(__TYPE__, __PARENT__)								\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public CORE::ReflectionDescriptor		\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METhod(__TYPE__##ReflectionDescriptor)				\
																					\
private:																			\

///@brief 
#define DESCRIBE_REFLECTED_ABSTRACT_CLASS(__TYPE__)									\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()			\
: CORE::ReflectionDescriptor(#__TYPE__, nullptr)									\

///@brief 
#define DESCRIBE_REFLECTED_CLASS(__TYPE__)											\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()			\
: CORE::ReflectionDescriptor(#__TYPE__, nullptr)									\

///@brief 
#define DESCRIBE_REFLECTED_DERIVED_ABSTRACT_CLASS(__TYPE__, __PARENT__)				\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()			\
: CORE::ReflectionDescriptor(#__TYPE__, __PARENT__::GetReflectionDescriptor())		\

///@brief 
#define DESCRIBE_REFLECTED_DERIVED_CLASS(__TYPE__, __PARENT__)						\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()			\
: CORE::ReflectionDescriptor(#__TYPE__, __PARENT__::GetReflectionDescriptor())		\

