#pragma once
#include "utility.h"

enum CurveType
{
	kLinear = 1,
	kLogarithmic,
	kSine
};

/**
 * \brief Apply curve for value
 * \param x value between 0 and 1
 * \param curve_type curve to apply
 */
[[nodiscard]] inline float ApplyCurve(float x, CurveType curve_type)
{
	x = std::clamp(x, 0.f, 1.f);

	switch (curve_type)
	{
		case kLogarithmic: return log(1.f / (1.f - (exp(1.f) - 1.f) * (x - 1.f))) + 1.f;
		case kSine: return pow(sin((5.f * x) / kPi), 2);
		case kLinear: return x;
		default: throw std::exception("Unknown curve type");
	}
}
