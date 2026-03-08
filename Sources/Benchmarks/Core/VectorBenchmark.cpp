#include <benchmark/benchmark.h>
#include <HodEngine/Core/Vector.hpp>
#include <vector>

// ============================================================================
// PushBack — without pre-allocation (triggers reallocations)
// ============================================================================

static void Hod_Vector_PushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::Vector<int> v;
		for (int i = 0; i < state.range(0); ++i)
			v.PushBack(i);
		benchmark::DoNotOptimize(v.Data());
	}
}
BENCHMARK(Hod_Vector_PushBack)->Range(64, 1 << 16);

static void Std_Vector_PushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v;
		for (int i = 0; i < state.range(0); ++i)
			v.push_back(i);
		benchmark::DoNotOptimize(v.data());
	}
}
BENCHMARK(Std_Vector_PushBack)->Range(64, 1 << 16);

// ============================================================================
// PushBack — with pre-allocation (no reallocation)
// ============================================================================

static void Hod_Vector_PushBack_Reserved(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::Vector<int> v;
		v.Reserve(static_cast<uint32_t>(state.range(0)));
		for (int i = 0; i < state.range(0); ++i)
			v.PushBack(i);
		benchmark::DoNotOptimize(v.Data());
	}
}
BENCHMARK(Hod_Vector_PushBack_Reserved)->Range(64, 1 << 16);

static void Std_Vector_PushBack_Reserved(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<int> v;
		v.reserve(static_cast<size_t>(state.range(0)));
		for (int i = 0; i < state.range(0); ++i)
			v.push_back(i);
		benchmark::DoNotOptimize(v.data());
	}
}
BENCHMARK(Std_Vector_PushBack_Reserved)->Range(64, 1 << 16);

// ============================================================================
// EmplaceBack — non-trivial type
// ============================================================================

struct NonTrivial
{
	int   x, y, z;
	float w;
	NonTrivial(int x, int y, int z, float w) : x(x), y(y), z(z), w(w) {}
};

static void Hod_Vector_EmplaceBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::Vector<NonTrivial> v;
		for (int i = 0; i < state.range(0); ++i)
			v.EmplaceBack(i, i, i, static_cast<float>(i));
		benchmark::DoNotOptimize(v.Data());
	}
}
BENCHMARK(Hod_Vector_EmplaceBack)->Range(64, 1 << 14);

static void Std_Vector_EmplaceBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<NonTrivial> v;
		for (int i = 0; i < state.range(0); ++i)
			v.emplace_back(i, i, i, static_cast<float>(i));
		benchmark::DoNotOptimize(v.data());
	}
}
BENCHMARK(Std_Vector_EmplaceBack)->Range(64, 1 << 14);

// ============================================================================
// Iteration
// ============================================================================

static void Hod_Vector_Iterate(benchmark::State& state)
{
	hod::Vector<int> v;
	v.Reserve(static_cast<uint32_t>(state.range(0)));
	for (int i = 0; i < state.range(0); ++i)
		v.PushBack(i);

	for (auto _ : state)
	{
		int64_t sum = 0;
		for (int x : v)
			sum += x;
		benchmark::DoNotOptimize(sum);
	}
}
BENCHMARK(Hod_Vector_Iterate)->Range(64, 1 << 16);

static void Std_Vector_Iterate(benchmark::State& state)
{
	std::vector<int> v;
	v.reserve(static_cast<size_t>(state.range(0)));
	for (int i = 0; i < state.range(0); ++i)
		v.push_back(i);

	for (auto _ : state)
	{
		int64_t sum = 0;
		for (int x : v)
			sum += x;
		benchmark::DoNotOptimize(sum);
	}
}
BENCHMARK(Std_Vector_Iterate)->Range(64, 1 << 16);

// ============================================================================
// Erase at beginning (worst case: shifts all elements)
// ============================================================================

static void Hod_Vector_EraseBegin(benchmark::State& state)
{
	const int count = static_cast<int>(state.range(0));
	for (auto _ : state)
	{
		hod::Vector<int> v;
		v.Reserve(static_cast<uint32_t>(count));
		for (int i = 0; i < count; ++i)
			v.PushBack(i);

		while (!v.Empty())
			v.Erase(v.Begin());

		benchmark::DoNotOptimize(v.Data());
	}
}
BENCHMARK(Hod_Vector_EraseBegin)->Range(64, 1 << 12);

static void Std_Vector_EraseBegin(benchmark::State& state)
{
	const int count = static_cast<int>(state.range(0));
	for (auto _ : state)
	{
		std::vector<int> v;
		v.reserve(static_cast<size_t>(count));
		for (int i = 0; i < count; ++i)
			v.push_back(i);

		while (!v.empty())
			v.erase(v.begin());

		benchmark::DoNotOptimize(v.data());
	}
}
BENCHMARK(Std_Vector_EraseBegin)->Range(64, 1 << 12);

// ============================================================================
// Copy
// ============================================================================

static void Hod_Vector_Copy(benchmark::State& state)
{
	hod::Vector<int> src;
	src.Reserve(static_cast<uint32_t>(state.range(0)));
	for (int i = 0; i < state.range(0); ++i)
		src.PushBack(i);

	for (auto _ : state)
	{
		hod::Vector<int> copy(src);
		benchmark::DoNotOptimize(copy.Data());
	}
}
BENCHMARK(Hod_Vector_Copy)->Range(64, 1 << 16);

static void Std_Vector_Copy(benchmark::State& state)
{
	std::vector<int> src;
	src.reserve(static_cast<size_t>(state.range(0)));
	for (int i = 0; i < state.range(0); ++i)
		src.push_back(i);

	for (auto _ : state)
	{
		std::vector<int> copy(src);
		benchmark::DoNotOptimize(copy.data());
	}
}
BENCHMARK(Std_Vector_Copy)->Range(64, 1 << 16);
