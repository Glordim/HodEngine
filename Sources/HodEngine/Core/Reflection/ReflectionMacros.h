#pragma once

#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"
#include "HodEngine/Core/Reflection/ReflectionHelper.h"
#include "HodEngine/Game/Traits/ReflectionTraitCustomEditor.h" // todo move

///@brief Implement Getter method to retreive ReflectionDescriptor
#define GET_REFLECTION_DESCRIPTOR_METHOD(__REFLECTION_DESCRIPTOR_TYPE__)		\
static __REFLECTION_DESCRIPTOR_TYPE__*	GetReflectionDescriptor()				\
{																				\
	static __REFLECTION_DESCRIPTOR_TYPE__ reflectionDescriptor;					\
	return &reflectionDescriptor;												\
}																				\

///@brief Declare sub class used for Reflection
#define REFLECTED_CLASS_NO_VIRTUAL(__TYPE__)										\
																					\
	META_TYPE_NO_VIRTUAL(__TYPE__)													\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public hod::ReflectionDescriptor			\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METHOD(__TYPE__##ReflectionDescriptor)				\
																					\
	hod::ReflectionDescriptor* GetReflectionDescriptorV() const						\
	{																				\
		return __TYPE__::GetReflectionDescriptor();									\
	}																				\
																					\
private:																			\

///@brief Declare sub class used for Reflection
#define REFLECTED_CLASS_NO_PARENT(__TYPE__)											\
																					\
	BASE_META_TYPE(__TYPE__)														\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public hod::ReflectionDescriptor			\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METHOD(__TYPE__##ReflectionDescriptor)				\
																					\
	virtual hod::ReflectionDescriptor* GetReflectionDescriptorV() const				\
	{																				\
		return __TYPE__::GetReflectionDescriptor();									\
	}																				\
																					\
private:																			\

///@brief Declare sub class used for Reflection
#define REFLECTED_CLASS(__TYPE__, __PARENT__)										\
																					\
	META_TYPE(__TYPE__, __PARENT__)													\
																					\
public:																				\
																					\
	class __TYPE__##ReflectionDescriptor : public hod::ReflectionDescriptor			\
	{																				\
		public:																		\
																					\
			__TYPE__##ReflectionDescriptor();										\
	};																				\
																					\
	GET_REFLECTION_DESCRIPTOR_METHOD(__TYPE__##ReflectionDescriptor)				\
																					\
	hod::ReflectionDescriptor* GetReflectionDescriptorV() const override			\
	{																				\
		return __TYPE__::GetReflectionDescriptor();									\
	}																				\
																					\
private:																			\

///@brief 
#define DESCRIBE_REFLECTED_CLASS_NO_PARENT(__TYPE__)														\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()									\
: hod::ReflectionDescriptor(hod::ReflectionDescriptor::GenerateReflectionData<__TYPE__, void>(#__TYPE__))	\

///@brief 
#define DESCRIBE_REFLECTED_CLASS(__TYPE__, __PARENT__)																\
__TYPE__::__TYPE__##ReflectionDescriptor::__TYPE__##ReflectionDescriptor()											\
: hod::ReflectionDescriptor(hod::ReflectionDescriptor::GenerateReflectionData<__TYPE__, __PARENT__>(#__TYPE__))		\

#define ADD_PROPERTY(Class, Member) hod::ReflectionHelper::AddProperty<decltype(Class::Member)>(this, #Member, offsetof(Class, Member), nullptr)

#define ADD_PROPERTY_WITH_SET_METHOD(Class, Member, SetMethod) hod::ReflectionHelper::AddProperty<decltype(Class::Member)>(this, #Member, offsetof(Class, Member), [](void* instance, void* value){static_cast<Class*>(instance)->SetMethod(*reinterpret_cast<decltype(Class::Member)*>(value));})

 // todo move
#if defined(HOD_EDITOR)
#define ADD_CUSTOM_EDITOR(Class) AddTrait<ReflectionTraitCustomEditor>(new Class())
#else
#define ADD_CUSTOM_EDITOR(Class) 
#endif
