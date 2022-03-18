#pragma once

#include "Hash.h"

#define BASE_META_TYPE(__ClassName__)										\
public:																		\
																			\
using Type = uint64_t;														\
																			\
virtual Type		GetType() const = 0;									\
virtual const char*	GetTypeName() const = 0;								\
																			\
private:																	\
																			\


#define META_TYPE(__ClassName__)											\
public:																		\
																			\
using Type = uint64_t;														\
																			\
static constexpr Type _type = Hash::CompilationTimeFnv64(#__ClassName__);	\
Type GetType() const override												\
{																			\
	return Hash::CompilationTimeFnv64(#__ClassName__);						\
}																			\
																			\
const char* GetTypeName() const override									\
{																			\
	return #__ClassName__;													\
}																			\
																			\
static const char* GetTypeNameStatic()										\
{																			\
	return #__ClassName__;													\
}																			\
																			\
private:																	\
																			\
