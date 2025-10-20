#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/String.hpp"

#include <format>
#include <string_view>

namespace hod
{
	class HOD_CORE_API Path
	{
	public:
		static constexpr char PreferredSeparator =
#if defined(PLATFORM_WINDOWS)
			'\\';
#else
			'/';
#endif

		static constexpr char GenericSeparator = '/';

	public:
		Path();
		Path(const char* path);
		Path(const String& path);
		Path(const std::string_view& path);
		Path(const Path& other);
		Path(Path&& other) noexcept;
		~Path() = default;

		Path& operator=(const char* path);
		Path& operator=(const String& path);
		Path& operator=(const Path& other);
		Path& operator=(Path&& other) noexcept;

		// Concatenation operators
		Path  operator/(const Path& other) const;
		Path  operator/(const char* other) const;
		Path  operator/(const String& other) const;
		Path& operator/=(const Path& other);
		Path& operator/=(const char* other);
		Path& operator/=(const String& other);

		Path  operator+(const Path& other) const;
		Path  operator+(const char* other) const;
		Path  operator+(const String& other) const;
		Path& operator+=(const Path& other);
		Path& operator+=(const char* other);
		Path& operator+=(const String& other);

		// Comparison operators
		bool operator==(const Path& other) const;
		bool operator!=(const Path& other) const;
		bool operator<(const Path& other) const;

		// String conversion
					  operator std::string_view() const;
		const String& GetString() const;
		const char*   CStr() const;

		// Path manipulation
		Path& Append(const Path& other);
		Path& Append(const char* other);
		Path& Append(const String& other);

		Path& Concat(const Path& other);
		Path& Concat(const char* other);
		Path& Concat(const String& other);

		Path& Clear();
		Path& MakePreferred(); // Convert separators to platform preferred
		Path& RemoveFilename();
		Path& ReplaceFilename(const Path& replacement);
		Path& ReplaceExtension(const Path& replacement = Path());

		// Path decomposition
		Path RootName() const;
		Path RootDirectory() const;
		Path RootPath() const;
		Path RelativePath() const;
		Path ParentPath() const;
		Path Filename() const;
		Path Stem() const;
		Path Extension() const;

		// Path queries
		bool Empty() const;
		bool HasRootName() const;
		bool HasRootDirectory() const;
		bool HasRootPath() const;
		bool HasRelativePath() const;
		bool HasParentPath() const;
		bool HasFilename() const;
		bool HasStem() const;
		bool HasExtension() const;
		bool IsAbsolute() const;
		bool IsRelative() const;

		// Lexically normalize path
		Path LexicallyNormal() const;
		Path LexicallyRelative(const Path& base) const;
		Path LexicallyProximate(const Path& base) const;

		// Native path conversion (for OS APIs)
#if defined(PLATFORM_WINDOWS)
		std::wstring ToNative() const; // Convert UTF-8 to UTF-16 for Windows
		static Path  FromNative(const wchar_t* nativePath);
#else
		const String& ToNative() const
		{
			return _path;
		} // Already UTF-8 on Unix

		static Path FromNative(const char* nativePath)
		{
			return Path(nativePath);
		}
#endif

		// Utility
		static bool IsSeparator(char c);
		static Path Current(); // Get current working directory
		static Path Temp();    // Get temp directory

	private:
		void     NormalizeSeparators();
		uint32_t FindFilenamePos() const;
		uint32_t FindExtensionPos() const;
		uint32_t FindRootNameEnd() const;

		String _path;
	};

	// Non-member operators
	Path operator/(const char* lhs, const Path& rhs);
	Path operator/(const String& lhs, const Path& rhs);

	Path operator+(const char* lhs, const Path& rhs);
	Path operator+(const String& lhs, const Path& rhs);
}

// std::hash specialization
template<>
struct std::hash<hod::Path>
{
	std::size_t operator()(const hod::Path& path) const noexcept
	{
		return std::hash<hod::String> {}(path.GetString());
	}
};

// std::formatter specialization
template<>
struct std::formatter<hod::Path, char> : std::formatter<std::string_view, char>
{
	constexpr formatter() noexcept = default;

	auto format(const hod::Path& path, auto& ctx) const
	{
		return std::formatter<std::string_view, char>::format(path.GetString(), ctx);
	}
};
