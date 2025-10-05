#pragma once

#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/ReflectionHelper.hpp"

#define REFLECTED_CLASS_NO_PARENT(CLASS)                                                                             \
	BASE_META_TYPE(CLASS)                                                                                            \
public:                                                                                                              \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)                            \
	{                                                                                                                \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS>(#CLASS));                 \
		FillReflectionDescriptorUser(reflectionDescriptor);                                                          \
	}                                                                                                                \
	static void                       FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor); \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                                      \
	{                                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                                \
	}                                                                                                                \
	virtual ReflectionDescriptor& GetReflectionDescriptorV() const                                                   \
	{                                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                                \
	}

#define REFLECTED_CLASS_NO_VIRTUAL(CLASS)                                                                            \
	META_TYPE_NO_VIRTUAL(CLASS)                                                                                      \
public:                                                                                                              \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)                            \
	{                                                                                                                \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS>(#CLASS));                 \
		FillReflectionDescriptorUser(reflectionDescriptor);                                                          \
	}                                                                                                                \
	static void                       FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor); \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                                      \
	{                                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                                \
	}                                                                                                                \
	ReflectionDescriptor& GetReflectionDescriptorV() const                                                           \
	{                                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                                \
	}

#define REFLECTED_CLASS(CLASS, PARENT)                                                                               \
	META_TYPE(CLASS, PARENT)                                                                                         \
public:                                                                                                              \
	static void FillReflectionDescriptor(hod::ReflectionDescriptor& reflectionDescriptor)                            \
	{                                                                                                                \
		reflectionDescriptor.Init(hod::ReflectionDescriptor::GenerateReflectionData<CLASS, PARENT>(#CLASS));         \
		FillReflectionDescriptorUser(reflectionDescriptor);                                                          \
	}                                                                                                                \
	static void                       FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor); \
	static hod::ReflectionDescriptor& GetReflectionDescriptor()                                                      \
	{                                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                                \
	}                                                                                                                \
	hod::ReflectionDescriptor& GetReflectionDescriptorV() const override                                             \
	{                                                                                                                \
		return hod::ReflectedClass<CLASS>::GetReflectionDescriptor();                                                \
	}

///@brief
#define DESCRIBE_REFLECTED_CLASS(__TYPE__, reflectionDescriptor) void __TYPE__::FillReflectionDescriptorUser(hod::ReflectionDescriptor& reflectionDescriptor)

namespace hod
{
	template<typename _Class_>
	class ReflectedClass
	{
	public:
		static ReflectionDescriptor& GetReflectionDescriptor()
		{
			static ReflectionDescriptor reflectionDescriptor;
			static bool                 init = false;
			if (init == false)
			{
				_Class_::FillReflectionDescriptor(reflectionDescriptor);
				init = true;
			}
			return reflectionDescriptor;
		}
	};

	// std::function<void(T::*)(const MemberType&)> setFunction = nullptr

	template<typename T, typename MemberType>
		requires(std::is_fundamental_v<MemberType> || std::is_enum_v<MemberType>)
	ReflectionProperty* AddPropertyT(ReflectionDescriptor& descriptor, MemberType T::* member, const char* name,
	                                 void (T::*setFunction)(MemberType) = nullptr /*, std::function<MemberType(void) const> getFunction = nullptr*/)
	{
		uint32_t offset = OffsetOf(member);

		if (setFunction != nullptr)
		{
			ReflectionProperty* property = ReflectionHelper::AddProperty<MemberType>(
				descriptor, name, offset,
				[setFunction](void* instance, void* value)
				{
					T*         instanceClass = static_cast<T*>(instance);
					MemberType valueCopy = *static_cast<MemberType*>(value);
					(instanceClass->*setFunction)(valueCopy);
				},
				nullptr);

			return property;
		}
		else
		{
			return ReflectionHelper::AddProperty<MemberType>(descriptor, name, offset);
		}
	}

	template<typename T, typename MemberType>
		requires(!std::is_fundamental_v<MemberType> && !std::is_enum_v<MemberType>)
	ReflectionProperty* AddPropertyT(ReflectionDescriptor& descriptor, MemberType T::* member, const char* name,
	                                 void (T::*setFunction)(const MemberType&) = nullptr /*, std::function<const MemberType&(void) const> getFunction = nullptr*/)
	{
		uint32_t offset = OffsetOf(member);
		if (setFunction != nullptr)
		{
			ReflectionProperty* property = ReflectionHelper::AddProperty<MemberType>(
				descriptor, name, offset,
				[setFunction](void* instance, void* value)
				{
					T*                instanceClass = static_cast<T*>(instance);
					const MemberType& valueRef = *static_cast<MemberType*>(value);
					(instanceClass->*setFunction)(valueRef);
				},
				nullptr);
			return property;
		}
		else
		{
			return ReflectionHelper::AddProperty<MemberType>(descriptor, name, offset);
		}
	}
}
