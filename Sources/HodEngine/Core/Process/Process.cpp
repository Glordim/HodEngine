#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Process/Process.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param desc
	/// @return
	bool Process::RunToOutput(Desc desc)
	{
		std::function<void(std::string_view, bool)> onOutputLine = std::move(desc._onOutputLine);
		desc._onOutputLine = [&onOutputLine](std::string_view line, bool error)
		{
			if (error)
			{
				OUTPUT_ERROR("{}", line);
			}
			else
			{
				OUTPUT_MESSAGE("{}", line);
			}
			if (onOutputLine != nullptr)
			{
				onOutputLine(line, error);
			}
		};

		Result result;
		if (Run(desc, result) == false)
		{
			OUTPUT_ERROR("{}", result._error);
			return false;
		}
		if (result._timedOut)
		{
			OUTPUT_ERROR("{}: no answer after {}s, stopped", std::string_view(desc._program), desc._timeoutMs / 1000);
		}
		return result._exitCode == 0;
	}

	/// @brief
	/// @param desc
	/// @param output
	/// @param emitted
	/// @param error
	/// @param end
	void Process::EmitLines(const Desc& desc, const String& output, uint32_t& emitted, bool error, bool end)
	{
		if (desc._onOutputLine == nullptr || emitted >= output.Size())
		{
			return;
		}

		auto emit = [&desc, error](std::string_view line)
		{
			if (line.empty() == false && line.back() == '\r')
			{
				line.remove_suffix(1);
			}
			desc._onOutputLine(line, error);
		};

		std::string_view pending(output.CStr() + emitted, output.Size() - emitted);
		for (size_t newline = pending.find('\n'); newline != std::string_view::npos; newline = pending.find('\n'))
		{
			emit(pending.substr(0, newline));
			emitted += static_cast<uint32_t>(newline + 1);
			pending.remove_prefix(newline + 1);
		}
		if (end && pending.empty() == false)
		{
			emit(pending);
			emitted = output.Size();
		}
	}
}
