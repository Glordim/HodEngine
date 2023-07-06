#pragma once

#include "Hash.h"

using MetaType = uint64_t;

#define BASE_META_TYPE(__ClassName__)												\
private:																			\
static constexpr MetaType _metaType = Hash::CompilationTimeFnv64(#__ClassName__);	\
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
static MetaType GetMetaTypeStatic()													\
{																					\
	return Hash::CompilationTimeFnv64(#__ClassName__);								\
}																					\
																					\
static const char* GetMetaTypeNameStatic()											\
{																					\
	return #__ClassName__;															\
}																					\
																					\
private:																			\
																					\


// TODO constexpr + rename Meta to Class

#define META_TYPE(__ClassName__)													\
private:																			\
static constexpr MetaType _metaType = Hash::CompilationTimeFnv64(#__ClassName__);	\
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
static MetaType GetMetaTypeStatic()													\
{																					\
	return Hash::CompilationTimeFnv64(#__ClassName__);								\
}																					\
																					\
static const char* GetMetaTypeNameStatic()											\
{																					\
	return #__ClassName__;															\
}																					\
																					\
private:																			\
																					\
