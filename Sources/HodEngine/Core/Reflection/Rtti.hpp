#pragma once

#include "HodEngine/Core/Hash.hpp"

using RttiType = uint64_t;

#define RTTI_BASE(__ClassName__)                                                      \
private:                                                                                   \
	static constexpr RttiType _rttiType = hod::Hash::CompilationTimeFnv64(#__ClassName__); \
                                                                                           \
public:                                                                                    \
	virtual RttiType GetRttiType() const                                                   \
	{                                                                                      \
		return GetRttiTypeStatic();                                                        \
	}                                                                                      \
                                                                                           \
	virtual const char* GetRttiTypeName() const                                            \
	{                                                                                      \
		return GetRttiTypeNameStatic();                                                    \
	}                                                                                      \
                                                                                           \
	static constexpr RttiType GetRttiTypeStatic()                                          \
	{                                                                                      \
		return hod::Hash::CompilationTimeFnv64(#__ClassName__);                            \
	}                                                                                      \
                                                                                           \
	static constexpr const char* GetRttiTypeNameStatic()                                   \
	{                                                                                      \
		return #__ClassName__;                                                             \
	}                                                                                      \
                                                                                           \
	virtual bool HasCompatibleRttiType(RttiType otherRttiType)                             \
	{                                                                                      \
		return (__ClassName__::GetRttiTypeStatic() == otherRttiType);                      \
	}                                                                                      \
                                                                                           \
private:

#define RTTI(__ClassName__, __ParentClass__)                                                                                      \
private:                                                                                                                               \
	static constexpr RttiType _rttiType = hod::Hash::CompilationTimeFnv64(#__ClassName__);                                             \
                                                                                                                                       \
public:                                                                                                                                \
	RttiType GetRttiType() const override                                                                                              \
	{                                                                                                                                  \
		return GetRttiTypeStatic();                                                                                                    \
	}                                                                                                                                  \
                                                                                                                                       \
	const char* GetRttiTypeName() const override                                                                                       \
	{                                                                                                                                  \
		return GetRttiTypeNameStatic();                                                                                                \
	}                                                                                                                                  \
                                                                                                                                       \
	static constexpr RttiType GetRttiTypeStatic()                                                                                      \
	{                                                                                                                                  \
		return hod::Hash::CompilationTimeFnv64(#__ClassName__);                                                                        \
	}                                                                                                                                  \
                                                                                                                                       \
	static constexpr const char* GetRttiTypeNameStatic()                                                                               \
	{                                                                                                                                  \
		return #__ClassName__;                                                                                                         \
	}                                                                                                                                  \
                                                                                                                                       \
	bool HasCompatibleRttiType(RttiType otherRttiType) override                                                                        \
	{                                                                                                                                  \
		return (__ClassName__::GetRttiTypeStatic() == otherRttiType || __ParentClass__::HasCompatibleRttiType(otherRttiType) == true); \
	}                                                                                                                                  \
                                                                                                                                       \
private:


#define RTTI_NO_VIRTUAL(__ClassName__)                                                \
private:                                                                                   \
	static constexpr RttiType _rttiType = hod::Hash::CompilationTimeFnv64(#__ClassName__); \
                                                                                           \
public:                                                                                    \
	RttiType GetRttiType() const                                                           \
	{                                                                                      \
		return GetRttiTypeStatic();                                                        \
	}                                                                                      \
                                                                                           \
	const char* GetRttiTypeName() const                                                    \
	{                                                                                      \
		return GetRttiTypeNameStatic();                                                    \
	}                                                                                      \
                                                                                           \
	static constexpr RttiType GetRttiTypeStatic()                                          \
	{                                                                                      \
		return hod::Hash::CompilationTimeFnv64(#__ClassName__);                            \
	}                                                                                      \
                                                                                           \
	static constexpr const char* GetRttiTypeNameStatic()                                   \
	{                                                                                      \
		return #__ClassName__;                                                             \
	}                                                                                      \
                                                                                           \
	bool HasCompatibleRttiType(RttiType otherRttiType)                                     \
	{                                                                                      \
		return (__ClassName__::GetRttiTypeStatic() == otherRttiType);                      \
	}                                                                                      \
                                                                                           \
private: