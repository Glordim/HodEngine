#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/Path.hpp"

#include <cstring>

#if defined(PLATFORM_WINDOWS)
	#include <win32/file.h> // GetTempPathW
	#include <win32/io.h>   // GetCurrentDirectoryW
	#include <win32/misc.h> // WideCharToMultiByte
#else
	#include <limits.h>
	#include <pwd.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

namespace hod
{
	Path::Path() {}

	Path::Path(const char* path)
	: _path(path)
	{
		NormalizeSeparators();
	}

	Path::Path(const String& path)
	: _path(path)
	{
		NormalizeSeparators();
	}

	Path::Path(const std::string_view& path)
	: _path(path)
	{
		NormalizeSeparators();
	}

	Path::Path(const Path& other)
	: _path(other._path)
	{
	}

	Path::Path(Path&& other) noexcept
	: _path(std::move(other._path))
	{
	}

	Path& Path::operator=(const char* path)
	{
		_path = path;
		NormalizeSeparators();
		return *this;
	}

	Path& Path::operator=(const String& path)
	{
		_path = path;
		NormalizeSeparators();
		return *this;
	}

	Path& Path::operator=(const Path& other)
	{
		_path = other._path;
		return *this;
	}

	Path& Path::operator=(Path&& other) noexcept
	{
		_path = std::move(other._path);
		return *this;
	}

	Path Path::operator/(const Path& other) const
	{
		Path result(*this);
		result.Append(other);
		return result;
	}

	Path Path::operator/(const char* other) const
	{
		Path result(*this);
		result.Append(other);
		return result;
	}

	Path Path::operator/(const String& other) const
	{
		Path result(*this);
		result.Append(other);
		return result;
	}

	Path& Path::operator/=(const Path& other)
	{
		return Append(other);
	}

	Path& Path::operator/=(const char* other)
	{
		return Append(other);
	}

	Path& Path::operator/=(const String& other)
	{
		return Append(other);
	}

	Path Path::operator+(const Path& other) const
	{
		Path result(*this);
		result.Concat(other);
		return result;
	}

	Path Path::operator+(const char* other) const
	{
		Path result(*this);
		result.Concat(other);
		return result;
	}

	Path Path::operator+(const String& other) const
	{
		Path result(*this);
		result.Concat(other);
		return result;
	}

	Path& Path::operator+=(const Path& other)
	{
		return Concat(other);
	}

	Path& Path::operator+=(const char* other)
	{
		return Concat(other);
	}

	Path& Path::operator+=(const String& other)
	{
		return Concat(other);
	}

	bool Path::operator==(const Path& other) const
	{
		return _path == other._path;
	}

	bool Path::operator!=(const Path& other) const
	{
		return _path != other._path;
	}

	bool Path::operator<(const Path& other) const
	{
		return _path < other._path;
	}

	Path::operator std::string_view() const
	{
		return std::string_view(_path);
	}

	const String& Path::GetString() const
	{
		return _path;
	}

	const char* Path::CStr() const
	{
		return _path.CStr();
	}

	Path& Path::Append(const Path& other)
	{
		return Append(other._path);
	}

	Path& Path::Append(const char* other)
	{
		if (other == nullptr || other[0] == '\0')
		{
			return *this;
		}

		return Append(String(other));
	}

	Path& Path::Append(const String& other)
	{
		if (other.Empty())
		{
			return *this;
		}

		// If other is absolute, replace current path
		Path otherPath(other);
		if (otherPath.IsAbsolute())
		{
			_path = other;
			NormalizeSeparators();
			return *this;
		}

		// Add separator if needed
		if (!_path.Empty() && !IsSeparator(_path.Back()) && !IsSeparator(other.Front()))
		{
			_path += PreferredSeparator;
		}

		_path += other;
		NormalizeSeparators();
		return *this;
	}

	Path& Path::Concat(const Path& other)
	{
		return Concat(other._path);
	}

	Path& Path::Concat(const char* other)
	{
		if (other != nullptr)
		{
			_path += other;
			NormalizeSeparators();
		}
		return *this;
	}

	Path& Path::Concat(const String& other)
	{
		_path += other;
		NormalizeSeparators();
		return *this;
	}

	Path& Path::Clear()
	{
		_path.Clear();
		return *this;
	}

	Path& Path::MakePreferred()
	{
		for (uint32_t i = 0; i < _path.Size(); ++i)
		{
			if (IsSeparator(_path[i]))
			{
				_path[i] = PreferredSeparator;
			}
		}
		return *this;
	}

	Path& Path::RemoveFilename()
	{
		uint32_t pos = FindFilenamePos();
		if (pos != String::Npos)
		{
			_path.Erase(pos);
		}
		return *this;
	}

	Path& Path::ReplaceFilename(const Path& replacement)
	{
		RemoveFilename();
		return Append(replacement);
	}

	Path& Path::ReplaceExtension(const Path& replacement)
	{
		uint32_t extPos = FindExtensionPos();
		if (extPos != String::Npos)
		{
			_path.Erase(extPos);
		}

		if (!replacement.Empty())
		{
			if (replacement._path[0] != '.')
			{
				_path += '.';
			}
			_path += replacement._path;
		}

		return *this;
	}

	Path Path::RootName() const
	{
#if defined(PLATFORM_WINDOWS)
		// Windows: "C:" or "\\server\share"
		if (_path.Size() >= 2)
		{
			// Drive letter: "C:"
			if (_path[1] == ':')
			{
				return Path(String(_path, 0, 2));
			}

			// UNC path: "\\server\share"
			if (IsSeparator(_path[0]) && IsSeparator(_path[1]))
			{
				uint32_t pos = 2;
				// Skip server name
				while (pos < _path.Size() && !IsSeparator(_path[pos]))
				{
					++pos;
				}

				if (pos < _path.Size())
				{
					++pos; // Skip separator
					// Skip share name
					while (pos < _path.Size() && !IsSeparator(_path[pos]))
					{
						++pos;
					}

					return Path(String(_path, 0, pos));
				}
			}
		}
#endif
		return Path();
	}

	Path Path::RootDirectory() const
	{
		uint32_t rootEnd = FindRootNameEnd();
		if (rootEnd < _path.Size() && IsSeparator(_path[rootEnd]))
		{
			return Path(String(1, PreferredSeparator));
		}
		return Path();
	}

	Path Path::RootPath() const
	{
		uint32_t rootEnd = FindRootNameEnd();
		if (rootEnd < _path.Size() && IsSeparator(_path[rootEnd]))
		{
			++rootEnd;
		}

		if (rootEnd > 0)
		{
			return Path(String(_path, 0, rootEnd));
		}

		return Path();
	}

	Path Path::RelativePath() const
	{
		uint32_t rootEnd = FindRootNameEnd();
		if (rootEnd < _path.Size() && IsSeparator(_path[rootEnd]))
		{
			++rootEnd;
		}

		if (rootEnd < _path.Size())
		{
			return Path(String(_path, rootEnd));
		}

		return Path();
	}

	Path Path::ParentPath() const
	{
		uint32_t pos = FindFilenamePos();
		if (pos == String::Npos || pos == 0)
		{
			return Path();
		}

		// Remove trailing separator
		while (pos > 0 && IsSeparator(_path[pos - 1]))
		{
			--pos;
		}

		if (pos == 0)
		{
			return Path();
		}

		return Path(String(_path, 0, pos));
	}

	Path Path::Filename() const
	{
		uint32_t pos = FindFilenamePos();
		if (pos != String::Npos)
		{
			return Path(String(_path, pos));
		}
		return Path(_path);
	}

	Path Path::Stem() const
	{
		Path filename = Filename();
		if (filename.Empty())
		{
			return Path();
		}

		uint32_t extPos = filename._path.FindR('.');
		if (extPos == String::Npos || extPos == 0)
		{
			return filename;
		}

		// Special case: ".hidden" files have no stem
		if (extPos == 0 && filename._path.Size() > 1)
		{
			return Path();
		}

		return Path(String(filename._path, 0, extPos));
	}

	Path Path::Extension() const
	{
		uint32_t extPos = FindExtensionPos();
		if (extPos != String::Npos)
		{
			return Path(String(_path, extPos));
		}
		return Path();
	}

	bool Path::Empty() const
	{
		return _path.Empty();
	}

	bool Path::HasRootName() const
	{
		return !RootName().Empty();
	}

	bool Path::HasRootDirectory() const
	{
		return !RootDirectory().Empty();
	}

	bool Path::HasRootPath() const
	{
		return !RootPath().Empty();
	}

	bool Path::HasRelativePath() const
	{
		return !RelativePath().Empty();
	}

	bool Path::HasParentPath() const
	{
		return !ParentPath().Empty();
	}

	bool Path::HasFilename() const
	{
		return !Filename().Empty();
	}

	bool Path::HasStem() const
	{
		return !Stem().Empty();
	}

	bool Path::HasExtension() const
	{
		return !Extension().Empty();
	}

	bool Path::IsAbsolute() const
	{
#if defined(PLATFORM_WINDOWS)
		// Windows: has root name (drive or UNC) and root directory
		return HasRootName() && HasRootDirectory();
#else
		// Unix: starts with '/'
		return !_path.Empty() && _path[0] == '/';
#endif
	}

	bool Path::IsRelative() const
	{
		return !IsAbsolute();
	}

	Path Path::LexicallyNormal() const
	{
		if (_path.Empty())
		{
			return Path();
		}

		Path   result;
		String rootPath = RootPath()._path;
		result._path = rootPath;

		String segment;

		uint32_t start = rootPath.Size();
		for (uint32_t i = start; i <= _path.Size(); ++i)
		{
			bool isSep = (i == _path.Size()) || IsSeparator(_path[i]);

			if (isSep && !segment.Empty())
			{
				if (segment == ".")
				{
					// Skip "." segments
				}
				else if (segment == "..")
				{
					// Handle ".." segments
					if (!result._path.Empty() && result._path.Back() != PreferredSeparator)
					{
						result.RemoveFilename();
					}
					else if (IsRelative())
					{
						if (!result._path.Empty())
						{
							result._path += PreferredSeparator;
						}
						result._path += "..";
					}
				}
				else
				{
					// Normal segment
					if (!result._path.Empty() && result._path.Back() != PreferredSeparator)
					{
						result._path += PreferredSeparator;
					}
					result._path += segment;
				}
				segment.Clear();
			}
			else if (!isSep)
			{
				segment += _path[i];
			}
		}

		return result;
	}

	Path Path::LexicallyRelative(const Path& base) const
	{
		// Simplified implementation
		Path normalThis = LexicallyNormal();
		Path normalBase = base.LexicallyNormal();

		if (normalThis._path.StartsWith(normalBase._path))
		{
			uint32_t pos = normalBase._path.Size();
			if (pos < normalThis._path.Size() && IsSeparator(normalThis._path[pos]))
			{
				++pos;
			}
			return Path(String(normalThis._path, pos));
		}

		return Path();
	}

	Path Path::LexicallyProximate(const Path& base) const
	{
		Path relative = LexicallyRelative(base);
		if (relative.Empty())
		{
			return *this;
		}
		return relative;
	}

#if defined(PLATFORM_WINDOWS)
	std::wstring Path::ToNative() const
	{
		if (_path.Empty())
		{
			return std::wstring();
		}

		// Convert UTF-8 to UTF-16
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _path.CStr(), static_cast<int>(_path.Size()), nullptr, 0);
		if (sizeNeeded <= 0)
		{
			return std::wstring();
		}

		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, _path.CStr(), static_cast<int>(_path.Size()), &result[0], sizeNeeded);

		return result;
	}

	Path Path::FromNative(const wchar_t* nativePath)
	{
		if (nativePath == nullptr || nativePath[0] == L'\0')
		{
			return Path();
		}

		// Convert UTF-16 to UTF-8
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, nativePath, -1, nullptr, 0, nullptr, nullptr);
		if (sizeNeeded <= 0)
		{
			return Path();
		}

		String result(sizeNeeded - 1); // -1 for null terminator
		WideCharToMultiByte(CP_UTF8, 0, nativePath, -1, result.Data(), sizeNeeded, nullptr, nullptr);

		return Path(result);
	}
#endif

	//-----------------------------------------------------------------------------
	// Utility methods
	//-----------------------------------------------------------------------------

	bool Path::IsSeparator(char c)
	{
		return c == '/' || c == '\\';
	}

	Path Path::Current()
	{
#if defined(PLATFORM_WINDOWS)
		wchar_t buffer[MAX_PATH];
		DWORD   Size = GetCurrentDirectoryW(MAX_PATH, buffer);
		if (Size > 0)
		{
			return FromNative(buffer);
		}
#else
		char buffer[PATH_MAX];
		if (getcwd(buffer, PATH_MAX) != nullptr)
		{
			return Path(buffer);
		}
#endif
		return Path();
	}

	Path Path::Temp()
	{
#if defined(PLATFORM_WINDOWS)
		wchar_t buffer[MAX_PATH];
		DWORD   Size = GetTempPathW(MAX_PATH, buffer);
		if (Size > 0)
		{
			return FromNative(buffer);
		}
#else
		const char* tmpDir = std::getenv("TMPDIR");
		if (tmpDir == nullptr)
		{
			tmpDir = std::getenv("TMP");
		}
		if (tmpDir == nullptr)
		{
			tmpDir = std::getenv("TEMP");
		}
		if (tmpDir == nullptr)
		{
			tmpDir = "/tmp";
		}

		return Path(tmpDir);
#endif
		return Path();
	}

	//-----------------------------------------------------------------------------
	// Private helper methods
	//-----------------------------------------------------------------------------

	void Path::NormalizeSeparators()
	{
		// Convert all separators to preferred separator
		for (uint32_t i = 0; i < _path.Size(); ++i)
		{
			if (IsSeparator(_path[i]))
			{
				_path[i] = PreferredSeparator;
			}
		}
	}

	uint32_t Path::FindFilenamePos() const
	{
		if (_path.Empty())
		{
			return String::Npos;
		}

		// Find last separator
		for (uint32_t i = _path.Size(); i > 0; --i)
		{
			if (IsSeparator(_path[i - 1]))
			{
				return i;
			}
		}

		return 0; // No separator found, entire path is filename
	}

	uint32_t Path::FindExtensionPos() const
	{
		uint32_t filenamePos = FindFilenamePos();
		if (filenamePos == String::Npos)
		{
			return String::Npos;
		}

		// Find last dot in filename
		uint32_t dotPos = _path.FindR('.');
		if (dotPos != String::Npos && dotPos > filenamePos)
		{
			// Special case: ".hidden" files
			if (dotPos == filenamePos)
			{
				return String::Npos;
			}

			return dotPos;
		}

		return String::Npos;
	}

	uint32_t Path::FindRootNameEnd() const
	{
#if defined(PLATFORM_WINDOWS)
		if (_path.Size() >= 2)
		{
			// Drive letter: "C:"
			if (_path[1] == ':')
			{
				return 2;
			}

			// UNC path: "\\server\share"
			if (IsSeparator(_path[0]) && IsSeparator(_path[1]))
			{
				uint32_t pos = 2;
				// Skip server name
				while (pos < _path.Size() && !IsSeparator(_path[pos]))
				{
					++pos;
				}

				if (pos < _path.Size())
				{
					++pos; // Skip separator
					// Skip share name
					while (pos < _path.Size() && !IsSeparator(_path[pos]))
					{
						++pos;
					}

					return pos;
				}
			}
		}
#endif
		return 0;
	}

	//-----------------------------------------------------------------------------
	// Non-member operators
	//-----------------------------------------------------------------------------

	Path operator/(const char* lhs, const Path& rhs)
	{
		return Path(lhs) / rhs;
	}

	Path operator/(const String& lhs, const Path& rhs)
	{
		return Path(lhs) / rhs;
	}

	Path operator+(const char* lhs, const Path& rhs)
	{
		Path result(lhs);
		result.Concat(rhs);
		return result;
	}

	Path operator+(const String& lhs, const Path& rhs)
	{
		Path result(lhs);
		result.Concat(rhs);
		return result;
	}
}