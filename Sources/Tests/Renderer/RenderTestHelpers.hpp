#pragma once

#include <gtest/gtest.h>

#include <HodEngine/Core/Output/OutputBucket.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <cmath>
#include <string>

constexpr uint32_t DefaultTargetSize = 64;
constexpr float    ColorTolerance = 2.0f / 255.0f;

// Clear color of CommandBuffer::StartRenderPass
inline const hod::Color Background(0.1f, 0.1f, 0.1f, 1.0f);

inline const hod::Color Red(1.0f, 0.0f, 0.0f, 1.0f);
inline const hod::Color Green(0.0f, 1.0f, 0.0f, 1.0f);
inline const hod::Color Blue(0.0f, 0.0f, 1.0f, 1.0f);
inline const hod::Color Yellow(1.0f, 1.0f, 0.0f, 1.0f);
inline const hod::Color White(1.0f, 1.0f, 1.0f, 1.0f);
inline const hod::Color Black(0.0f, 0.0f, 0.0f, 1.0f);

// What the RHI draws for a texture slot left without a texture
inline const hod::Color FallbackColor(1.0f, 0.0f, 1.0f, 1.0f);

inline hod::Vector4 ToVector4(const hod::Color& color)
{
	return hod::Vector4(color.r, color.g, color.b, color.a);
}

inline ::testing::AssertionResult ColorNear(const hod::Color& actual, const hod::Color& expected)
{
	if (std::abs(actual.r - expected.r) <= ColorTolerance && std::abs(actual.g - expected.g) <= ColorTolerance && std::abs(actual.b - expected.b) <= ColorTolerance &&
	    std::abs(actual.a - expected.a) <= ColorTolerance)
	{
		return ::testing::AssertionSuccess();
	}
	return ::testing::AssertionFailure() << "got (" << actual.r << ", " << actual.g << ", " << actual.b << ", " << actual.a << "), expected (" << expected.r << ", " << expected.g
										 << ", " << expected.b << ", " << expected.a << ")";
}

// ============================================================================
// Fails the test on any error or warning output it did not announce,
// which covers what the graphics API validation reports.
// ============================================================================

class OutputCheckedTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		hod::OutputService::PushBucket(_outputs);
	}

	void TearDown() override
	{
		hod::OutputService::PopBucket();

		EXPECT_EQ(CountOutputs(hod::Output::Type::Error), _expectedErrorCount) << FirstOutput(hod::Output::Type::Error);
		EXPECT_EQ(CountOutputs(hod::Output::Type::Warning), _expectedWarningCount) << FirstOutput(hod::Output::Type::Warning);
	}

	uint32_t CountOutputs(hod::Output::Type type) const
	{
		uint32_t count = 0;
		for (const hod::Output& output : _outputs.GetOutputs())
		{
			if (output.GetType() == type)
			{
				++count;
			}
		}
		return count;
	}

	std::string FirstOutput(hod::Output::Type type) const
	{
		for (const hod::Output& output : _outputs.GetOutputs())
		{
			if (output.GetType() == type)
			{
				return output.GetContent().CStr();
			}
		}
		return "";
	}

protected:
	hod::OutputBucket _outputs;
	uint32_t          _expectedErrorCount = 0;
	uint32_t          _expectedWarningCount = 0;
};
