#pragma once

#include "Hash.hpp"

using MetaType = uint64_t;

#define BASE_META_TYPE(__ClassName__)												\
private:																			\
static constexpr MetaType _metaType = hod::Hash::CompilationTimeFnv64(#__ClassName__);	\
public:																				\
																					\
virtual MetaType GetMetaType() const												\
{																					\
	return GetMetaTypeStatic();														\
}																					\
																					\
virtual const char* GetMetaTypeName() const											\
{																					\
	return GetMetaTypeNameStatic();													\
}																					\
																					\
static constexpr MetaType GetMetaTypeStatic()										\
{																					\
	return hod::Hash::CompilationTimeFnv64(#__ClassName__);								\
}																					\
																					\
static constexpr const char* GetMetaTypeNameStatic()								\
{																					\
	return #__ClassName__;															\
}																					\
																					\
virtual bool HasCompatibleMetaType(MetaType otherMetaType)							\
{																					\
	return (__ClassName__::GetMetaTypeStatic() == otherMetaType);					\
}																					\
																					\
private:																			\
																					\


#define META_TYPE_NO_VIRTUAL(__ClassName__)											\
private:																			\
static constexpr MetaType _metaType = hod::Hash::CompilationTimeFnv64(#__ClassName__);	\
public:																				\
																					\
MetaType GetMetaType() const														\
{																					\
	return GetMetaTypeStatic();														\
}																					\
																					\
const char* GetMetaTypeName() const													\
{																					\
	return GetMetaTypeNameStatic();													\
}																					\
																					\
static constexpr MetaType GetMetaTypeStatic()										\
{																					\
	return hod::Hash::CompilationTimeFnv64(#__ClassName__);								\
}																					\
																					\
static constexpr const char* GetMetaTypeNameStatic()								\
{																					\
	return #__ClassName__;															\
}																					\
																					\
bool HasCompatibleMetaType(MetaType otherMetaType)									\
{																					\
	return (__ClassName__::GetMetaTypeStatic() == otherMetaType);					\
}																					\
																					\
private:																			\
																					\

// TODO constexpr + rename Meta to Class

#define META_TYPE(__ClassName__, __ParentClass__)									\
private:																			\
static constexpr MetaType _metaType = hod::Hash::CompilationTimeFnv64(#__ClassName__);	\
public:																				\
																					\
MetaType GetMetaType() const override												\
{																					\
	return GetMetaTypeStatic();														\
}																					\
																					\
const char* GetMetaTypeName() const override										\
{																					\
	return GetMetaTypeNameStatic();													\
}																					\
																					\
static constexpr MetaType GetMetaTypeStatic()										\
{																					\
	return hod::Hash::CompilationTimeFnv64(#__ClassName__);								\
}																					\
																					\
static constexpr const char* GetMetaTypeNameStatic()								\
{																					\
	return #__ClassName__;															\
}																					\
																					\
bool HasCompatibleMetaType(MetaType otherMetaType) override							\
{																					\
	return (__ClassName__::GetMetaTypeStatic() == otherMetaType ||					\
			__ParentClass__::HasCompatibleMetaType(otherMetaType) == true);			\
}																					\
																					\
private:																			\
																					
