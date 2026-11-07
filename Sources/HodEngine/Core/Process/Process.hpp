#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/FileSystem/Path.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <atomic>
#include <cstdint>
#include <string_view>

namespace hod::inline core
{
	class HOD_CORE_API Process
	{
	public:
		struct EnvironmentVariable
		{
			String _name;
			String _value;
		};

		struct Desc
		{
			String                      _program;          // Looked up in the PATH if not a path
			Vector<String>              _arguments;        // One per argument, quoted/escaped as the platform needs (spaces kept)
			Path                        _workingDirectory; // Empty: the current one
			Vector<EnvironmentVariable> _environment;      // Added to (or overriding) the inherited environment, removed from it if _value is empty

			std::string_view         _input;            // Written to stdin, then closed (p4 -x -, git commit -F -). Empty: stdin is null
			uint32_t                 _timeoutMs = 0;    // 0: none. Killed once reached
			const std::atomic<bool>* _cancel = nullptr; // Checked while it runs: killed once true
		};

		struct Result
		{
			int32_t _exitCode = -1;
			String  _output; // stdout, whole
			String  _error;  // stderr, whole
			bool    _timedOut = false;
			bool    _canceled = false;
		};

	public:
		static bool Create(const std::string_view& program, const std::string_view& argument,
		                   bool detach); // TODO argument should be pass as array because split string by space can split a textual argument with a space inside ?

		/// @brief Runs it and waits for its end, both outputs read concurrently (no deadlock on a full pipe), no console window
		/// @return false if it couldn't be started (program not found...), result._exitCode tells the rest
		static bool Run(const Desc& desc, Result& result);

		/// @brief Started and left running (a diff tool, an external client): its outputs are dropped
		static bool Launch(const Desc& desc);
	};
}
