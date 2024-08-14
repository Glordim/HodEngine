#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <functional>

namespace hod
{
	///@brief 
	class HOD_API ReflectionTraitGetValueForSerialization : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitGetValueForSerialization, ReflectionTrait)

	public:

													ReflectionTraitGetValueForSerialization(const std::function<void(const void*, void*)>& method, uint32_t size);
													ReflectionTraitGetValueForSerialization(const ReflectionTraitGetValueForSerialization& copy) = default;
													ReflectionTraitGetValueForSerialization(ReflectionTraitGetValueForSerialization&& move) = default;
													~ReflectionTraitGetValueForSerialization() = default;

		ReflectionTraitGetValueForSerialization&	operator = (const ReflectionTraitGetValueForSerialization& copy) = default;
		ReflectionTraitGetValueForSerialization&	operator = (ReflectionTraitGetValueForSerialization&& move) = default;

	public:

		void		GetValueForSerialization(const void* instance, void* value) const;
		uint32_t	GetSize() const { return _size; }

	private:

		std::function<void(const void*, void*)>	_method;
		uint32_t								_size = 0;
	};
}
