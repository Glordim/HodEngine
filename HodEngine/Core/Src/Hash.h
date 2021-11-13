#pragma once

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Hash
{
public:

	static constexpr uint64_t	FNV_64_PRIME = 1099511628211ULL;
	static constexpr uint64_t	FNV_64_BASIS = 14695981039346656037ULL;

public:

	static constexpr uint64_t	Fnv64(char character, const char* remaining, uint64_t hash) { return ((character == '\0') ? hash : Fnv64(remaining[0], remaining + 1, (hash * FNV_64_PRIME) ^ character)); }
	static constexpr uint64_t	CompilationTimeFnv64(const char* string) { return (Fnv64(string[0], string + 1, FNV_64_BASIS)); }

private:

	static uint64_t				ComputeFnv64(const std::string& string, uint64_t basis = FNV_64_BASIS);
	static uint64_t				ComputeFnv64(const void* buffer, uint32_t size, uint64_t basis = FNV_64_BASIS);
};
