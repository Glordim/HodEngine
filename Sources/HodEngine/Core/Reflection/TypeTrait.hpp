#pragma once

#include <array>
#include "HodEngine/Core/Vector.hpp"
#include <type_traits>

namespace hod
{
    template <class...> constexpr std::false_type always_false{};

    template <typename T>
    struct IsArray : std::false_type {};

    template <typename T, size_t size>
    struct IsArray<std::array<T, size>> : std::true_type {};

    template <typename T>
    struct IsVector : std::false_type {};

    template <typename T>
    struct IsVector<Vector<T>> : std::true_type {};

    template <typename, typename = void>
    struct HasEqualOperator : std::false_type {};

    template <typename T>
    struct HasEqualOperator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};

    template <typename T>
    struct ElementType;

    template <typename T, std::size_t N>
    struct ElementType<T[N]>
    {
        using type = T;
    };

    template <typename _Class_, typename _Member_>
	uint32_t OffsetOf(_Member_ _Class_::*M)
	{
		return static_cast<uint32_t>(reinterpret_cast<uint64_t>(&(((_Class_*)0)->*M)));
	};
}
