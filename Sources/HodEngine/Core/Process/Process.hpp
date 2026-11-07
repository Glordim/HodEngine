#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/FileSystem/Path.hpp"
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
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

			/// @brief Optional: each line of its stdout (error false) and stderr (error true) as it comes, without its end of line, on
			/// the thread of Run (a long build shown live in the console). The outputs are still kept whole in Result
			std::function<void(std::string_view line, bool error)> _onOutputLine;
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
		/// @brief Runs it and waits for its end, both outputs read concurrently (no deadlock on a full pipe), no console window
		/// @return false if it couldn't be started (program not found...), result._exitCode tells the rest
		static bool Run(const Desc& desc, Result& result);

		/// @brief Run for the tools run for the user (cmake, slangc): each line in the console as it comes (stdout as messages,
		/// stderr as errors), with the failure to start it
		/// @return true if it exited with 0
		static bool RunToOutput(Desc desc);

		/// @brief Started and left running (a diff tool, an external client): its outputs are dropped
		static bool Launch(const Desc& desc);

	private:
		/// @brief By Run, after each read: the lines of output after emitted given to desc._onOutputLine, emitted moved after them.
		/// end: the last one too, even without its end of line
		static void EmitLines(const Desc& desc, const String& output, uint32_t& emitted, bool error, bool end);
	};
}
