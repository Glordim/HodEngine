#include <benchmark/benchmark.h>
#include <HodEngine/Core/String.hpp>
#include <string>

// hod::String SSO threshold: 16 chars (SMALL_BUFFER_MAX_CAPACITY)
static constexpr const char* SHORT_STR = "hello";          // 5 chars  — SSO
static constexpr const char* SSO_STR   = "123456789012345"; // 15 chars — SSO (fits exactly)
static constexpr const char* LONG_STR  = "1234567890123456789012345678901234567890123456789012345678901234"; // 64 chars — heap

// ============================================================================
// Construction from C-string
// ============================================================================

static void Hod_String_ConstructShort(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::String s(SHORT_STR);
		benchmark::DoNotOptimize(s.CStr());
	}
}
BENCHMARK(Hod_String_ConstructShort);

static void Std_String_ConstructShort(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string s(SHORT_STR);
		benchmark::DoNotOptimize(s.data());
	}
}
BENCHMARK(Std_String_ConstructShort);

static void Hod_String_ConstructSSO(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::String s(SSO_STR);
		benchmark::DoNotOptimize(s.CStr());
	}
}
BENCHMARK(Hod_String_ConstructSSO);

static void Std_String_ConstructSSO(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string s(SSO_STR);
		benchmark::DoNotOptimize(s.data());
	}
}
BENCHMARK(Std_String_ConstructSSO);

static void Hod_String_ConstructLong(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::String s(LONG_STR);
		benchmark::DoNotOptimize(s.CStr());
	}
}
BENCHMARK(Hod_String_ConstructLong);

static void Std_String_ConstructLong(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string s(LONG_STR);
		benchmark::DoNotOptimize(s.data());
	}
}
BENCHMARK(Std_String_ConstructLong);

// ============================================================================
// Copy
// ============================================================================

static void Hod_String_CopyShort(benchmark::State& state)
{
	hod::String src(SHORT_STR);
	for (auto _ : state)
	{
		hod::String copy(src);
		benchmark::DoNotOptimize(copy.CStr());
	}
}
BENCHMARK(Hod_String_CopyShort);

static void Std_String_CopyShort(benchmark::State& state)
{
	std::string src(SHORT_STR);
	for (auto _ : state)
	{
		std::string copy(src);
		benchmark::DoNotOptimize(copy.data());
	}
}
BENCHMARK(Std_String_CopyShort);

static void Hod_String_CopySSO(benchmark::State& state)
{
	hod::String src(SSO_STR);
	for (auto _ : state)
	{
		hod::String copy(src);
		benchmark::DoNotOptimize(copy.CStr());
	}
}
BENCHMARK(Hod_String_CopySSO);

static void Std_String_CopySSO(benchmark::State& state)
{
	std::string src(SSO_STR);
	for (auto _ : state)
	{
		std::string copy(src);
		benchmark::DoNotOptimize(copy.data());
	}
}
BENCHMARK(Std_String_CopySSO);

static void Hod_String_CopyLong(benchmark::State& state)
{
	hod::String src(LONG_STR);
	for (auto _ : state)
	{
		hod::String copy(src);
		benchmark::DoNotOptimize(copy.CStr());
	}
}
BENCHMARK(Hod_String_CopyLong);

static void Std_String_CopyLong(benchmark::State& state)
{
	std::string src(LONG_STR);
	for (auto _ : state)
	{
		std::string copy(src);
		benchmark::DoNotOptimize(copy.data());
	}
}
BENCHMARK(Std_String_CopyLong);

// ============================================================================
// Append — accumulation
// ============================================================================

static void Hod_String_AppendShort(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::String s;
		for (int i = 0; i < state.range(0); ++i)
			s.Append("ab");
		benchmark::DoNotOptimize(s.CStr());
	}
}
BENCHMARK(Hod_String_AppendShort)->Range(8, 1 << 12);

static void Std_String_AppendShort(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string s;
		for (int i = 0; i < state.range(0); ++i)
			s.append("ab");
		benchmark::DoNotOptimize(s.data());
	}
}
BENCHMARK(Std_String_AppendShort)->Range(8, 1 << 12);

// ============================================================================
// PushBack — char by char
// ============================================================================

static void Hod_String_PushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		hod::String s;
		for (int i = 0; i < state.range(0); ++i)
			s.PushBack('x');
		benchmark::DoNotOptimize(s.CStr());
	}
}
BENCHMARK(Hod_String_PushBack)->Range(8, 1 << 12);

static void Std_String_PushBack(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::string s;
		for (int i = 0; i < state.range(0); ++i)
			s.push_back('x');
		benchmark::DoNotOptimize(s.data());
	}
}
BENCHMARK(Std_String_PushBack)->Range(8, 1 << 12);

// ============================================================================
// Find
// ============================================================================

static void Hod_String_FindHit(benchmark::State& state)
{
	hod::String s(LONG_STR);
	for (auto _ : state)
	{
		uint32_t pos = s.Find('9');
		benchmark::DoNotOptimize(pos);
	}
}
BENCHMARK(Hod_String_FindHit);

static void Std_String_FindHit(benchmark::State& state)
{
	std::string s(LONG_STR);
	for (auto _ : state)
	{
		size_t pos = s.find('9');
		benchmark::DoNotOptimize(pos);
	}
}
BENCHMARK(Std_String_FindHit);

static void Hod_String_FindMiss(benchmark::State& state)
{
	hod::String s(LONG_STR);
	for (auto _ : state)
	{
		uint32_t pos = s.Find('Z');
		benchmark::DoNotOptimize(pos);
	}
}
BENCHMARK(Hod_String_FindMiss);

static void Std_String_FindMiss(benchmark::State& state)
{
	std::string s(LONG_STR);
	for (auto _ : state)
	{
		size_t pos = s.find('Z');
		benchmark::DoNotOptimize(pos);
	}
}
BENCHMARK(Std_String_FindMiss);

// ============================================================================
// Concatenation operator+
// ============================================================================

static void Hod_String_Concat(benchmark::State& state)
{
	hod::String a("Hello, ");
	hod::String b("World!");
	for (auto _ : state)
	{
		hod::String c = a + b;
		benchmark::DoNotOptimize(c.CStr());
	}
}
BENCHMARK(Hod_String_Concat);

static void Std_String_Concat(benchmark::State& state)
{
	std::string a("Hello, ");
	std::string b("World!");
	for (auto _ : state)
	{
		std::string c = a + b;
		benchmark::DoNotOptimize(c.data());
	}
}
BENCHMARK(Std_String_Concat);

// ============================================================================
// SubStr
// ============================================================================

static void Hod_String_SubStr(benchmark::State& state)
{
	hod::String s(LONG_STR);
	for (auto _ : state)
	{
		hod::String sub = s.SubStr(10, 20);
		benchmark::DoNotOptimize(sub.CStr());
	}
}
BENCHMARK(Hod_String_SubStr);

static void Std_String_SubStr(benchmark::State& state)
{
	std::string s(LONG_STR);
	for (auto _ : state)
	{
		std::string sub = s.substr(10, 20);
		benchmark::DoNotOptimize(sub.data());
	}
}
BENCHMARK(Std_String_SubStr);
