#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod
{
	class ReflectionDescriptor;

	///@brief 
	class HOD_CORE_API ReflectionTraitAllowedClasses : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitAllowedClasses, ReflectionTrait)

	public:

										ReflectionTraitAllowedClasses() = default;
										ReflectionTraitAllowedClasses(const ReflectionTraitAllowedClasses& copy) = default;
										ReflectionTraitAllowedClasses(ReflectionTraitAllowedClasses&& move) = default;
										~ReflectionTraitAllowedClasses() = default;

		ReflectionTraitAllowedClasses&	operator = (const ReflectionTraitAllowedClasses& copy) = default;
		ReflectionTraitAllowedClasses&	operator = (ReflectionTraitAllowedClasses&& move) = default;

	public:

		const Vector<ReflectionDescriptor*>&	GetAllowedClasses() const;

		template<typename T>
		void										AddAllowedClass();
		void										AddAllowedClass(ReflectionDescriptor* reflectionDescriptor);

	protected:

		Vector<ReflectionDescriptor*>	_allowedClasses;
	};

	template<typename T>
	void ReflectionTraitAllowedClasses::AddAllowedClass()
	{
		AddAllowedClass(T::GetReflectionDescriptor());
	}
}
